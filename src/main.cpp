#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>
#include <filesystem>
#include <CLI/CLI.hpp>  // 添加CLI11头文件
#include "Core/PascalParse.h"
#include "AST/ASTBase.h"  // 包含 AST 基类
#include "LlvmVisitor.h"
#include <signal.h>

// 程序配置结构体
struct CompilerConfig {
    std::string inputFileName;
    std::string outputFileName;
    std::string logFileName;
    bool debugMode = false;
    bool generateLLVM = false;
};

// 前置声明
void initLogger(const std::string& logFileName, bool debugMode);
void signalHandler(int signal);
bool setupEnvironment(CompilerConfig& config);
std::string readSourceFile(const std::string& filePath);
bool compileSource(const std::string& sourceCode, const CompilerConfig& config);
bool generateOutput(std::shared_ptr<ProgramAST> root, const CompilerConfig& config);

int main(int argc, char* argv[]) {
    try {
        // 创建CLI应用
        CLI::App app{"Pascal到C编译器 (PasCC)"};
        
        // 定义配置结构
        CompilerConfig config;
        
        // 添加命令行选项
        app.add_option("-i,--input", config.inputFileName, "指定输入的Pascal源文件")
           ->check(CLI::ExistingFile);  // 验证文件存在
        
        app.add_option("-o,--output", config.outputFileName, "指定输出的C文件");
        
        app.add_flag("-d,--debug", config.debugMode, "启用调试日志");
        
        app.add_flag("-l,--llvm", config.generateLLVM, "生成LLVM IR（输出到llvm.ll）");
        
        // 解析命令行
        try {
            app.parse(argc, argv);
        } catch (const CLI::ParseError &e) {
            return app.exit(e);
        }
        
        // 设置默认输入文件
        if (config.inputFileName.empty()) {
            config.inputFileName = "./test.pas";
            std::cout << "使用默认输入文件: " << config.inputFileName << std::endl;
        }
        
        // 设置默认输出文件
        if (config.outputFileName.empty()) {
            if (config.inputFileName == "./test.pas") {
                config.outputFileName = "./res.c";
            } else {
                std::filesystem::path inputPath(config.inputFileName);
                config.outputFileName = inputPath.stem().string() + ".c";
            }
        }
        
        // 设置环境（日志系统、信号处理等）
        if (!setupEnvironment(config)) {
            return 1;
        }
        
        // 读取源代码
        std::string sourceCode;
        try {
            sourceCode = readSourceFile(config.inputFileName);
        } catch (const std::exception& e) {
            spdlog::error("无法读取源文件: {}", e.what());
            return 1;
        }
        
        // 编译源代码
        if (!compileSource(sourceCode, config)) {
            return 1;
        }
        
        spdlog::debug("编译过程完成");
        return 0;
    } catch (const std::exception& e) {
        spdlog::error("发生未处理的异常: {}", e.what());
        return 1;
    }
}

// 初始化日志系统
void initLogger(const std::string& logFileName, bool debugMode) {
    std::vector<spdlog::sink_ptr> sinks;
    
    // 控制台输出更美观的格式
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("%^[%l]%$ [%H:%M:%S.%e] %v");
    console_sink->set_level(debugMode ? spdlog::level::debug : spdlog::level::warn);
    sinks.push_back(console_sink);
    
    // 文件输出包含更详细的信息
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFileName, true);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^[%l]%$ [%t] %v");
    file_sink->set_level(debugMode ? spdlog::level::debug : spdlog::level::warn);
    sinks.push_back(file_sink);
    
    auto logger = std::make_shared<spdlog::logger>("pascc", sinks.begin(), sinks.end());
    logger->set_level(debugMode ? spdlog::level::debug : spdlog::level::warn);
    
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    
    // 设置spdlog的全局格式化配置
    spdlog::set_error_handler([](const std::string& msg) {
        std::cerr << "spdlog错误: " << msg << std::endl;
    });
    
    spdlog::flush_on(spdlog::level::err);  // 错误级别以上的日志立即刷新
    
    spdlog::info("日志系统初始化完成");
}

// 信号处理函数
void signalHandler(int signal) {
    spdlog::critical("收到信号 {}，程序即将终止", signal);
    spdlog::shutdown();
    exit(signal);
}

// 设置环境
bool setupEnvironment(CompilerConfig& config) {
    // 创建日志目录
    std::filesystem::path logDirPath;
    
    if (!config.inputFileName.empty()) {
        std::filesystem::path inputPath(config.inputFileName);
        logDirPath = "logs" / inputPath.stem();
        config.logFileName = (logDirPath / "compile.log").string();
    } else {
        logDirPath = "logs";
        config.logFileName = (logDirPath / "pascc.log").string();
    }
    
    try {
        std::filesystem::create_directories(logDirPath);
    } catch (const std::exception& e) {
        std::cerr << "无法创建日志目录: " << e.what() << std::endl;
        return false;
    }
    
    // 初始化日志系统
    initLogger(config.logFileName, config.debugMode);
    
    // 设置信号处理
    signal(SIGSEGV, signalHandler);
    
    spdlog::info("使用输入文件: '{}'", config.inputFileName);
    spdlog::info("使用输出文件: '{}'", config.outputFileName);
    
    return true;
}

// 读取源文件
std::string readSourceFile(const std::string& filePath) {
    spdlog::debug("读取源文件: {}", filePath);
    
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("无法打开输入文件 '" + filePath + "'");
    }
    
    std::ostringstream oss;
    oss << inputFile.rdbuf();
    inputFile.close();
    
    spdlog::debug("成功读取源文件，大小: {} 字节", oss.str().size());
    return oss.str();
}

// 编译源代码
bool compileSource(const std::string& sourceCode, const CompilerConfig& config) {
    spdlog::debug("开始编译源代码");
    
    // 调用解析器
    PascalParser parser;
    if (!parser.parse(sourceCode)) {
        spdlog::error("解析失败: {}", parser.getResult());
        return false;
    }
    
    spdlog::debug("解析成功，获取AST");
    
    try {
        auto root = std::dynamic_pointer_cast<ProgramAST>(getAST());
        if (!root) {
            spdlog::error("无法获取程序AST");
            return false;
        }
        
        spdlog::debug("打印AST结构");
        root->printAST();
        
        // 生成输出
        return generateOutput(root, config);
    } catch (const std::bad_cast& e) {
        spdlog::error("错误: 无法将AST转换为ProgramAST");
        return false;
    } catch (const std::exception& e) {
        spdlog::error("错误: {}", e.what());
        return false;
    }
}

// 生成输出文件
bool generateOutput(std::shared_ptr<ProgramAST> root, const CompilerConfig& config) {
    spdlog::debug("开始生成输出文件");
    
    // 生成C代码
    try {
        auto cCode = std::make_unique<CCodeGenVisitor>();
        root->accept(*cCode);
        
        std::ofstream outputFile(config.outputFileName);
        if (!outputFile) {
            spdlog::error("无法创建输出文件: '{}'", config.outputFileName);
            return false;
        }
        
        outputFile << cCode->output.str() << std::endl;
        outputFile.close();
        
        spdlog::info("C代码生成完成: '{}'", config.outputFileName);
        
        // 生成LLVM IR（如果需要）
        if (config.generateLLVM) {
            spdlog::debug("开始生成LLVM IR");
            
            auto llvmCode = std::make_unique<LlvmVisitor>();
            root->accept(*llvmCode);
            
            std::ofstream llvmOutputFile("llvm.ll");
            if (!llvmOutputFile) {
                spdlog::error("无法创建LLVM输出文件: 'llvm.ll'");
                return false;
            }
            
            llvmOutputFile << llvmCode->getIR() << std::endl;
            llvmOutputFile.close();
            
            spdlog::info("LLVM IR生成完成: 'llvm.ll'");
        }
        
        return true;
    } catch (const std::exception& e) {
        spdlog::error("生成输出时发生错误: {}", e.what());
        return false;
    }
}

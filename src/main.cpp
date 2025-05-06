#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>
#include <filesystem>
#include "Core/PascalParse.h"
#include "AST/ASTBase.h"  // 包含 AST 基类

void printUsage() {
    std::cout << "用法: pascc -i <pascal源文件> [-o <输出文件路径>] [-d]" << std::endl;
    std::cout << "示例: pascc -i example.pas -o output.c" << std::endl;
    std::cout << "选项: -d 启用调试日志" << std::endl;
}

void initLogger(const std::string& logFileName = "logs/pascc.log", bool debugMode = false) {
    // 创建多个sink
    std::vector<spdlog::sink_ptr> sinks;
    
    // 添加控制台sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%^%l%$] [%s:%#] %v");
    console_sink->set_level(spdlog::level::warn);
    sinks.push_back(console_sink);
    
    // 添加文件sink，使用传入的logFileName
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFileName, true);
    file_sink->set_pattern("[%^%l%$] [%s:%#] %v");
    file_sink->set_level(debugMode ? spdlog::level::debug : spdlog::level::warn);
    sinks.push_back(file_sink);
    
    // 创建多sink日志记录器
    auto logger = std::make_shared<spdlog::logger>("pascc", sinks.begin(), sinks.end());
    logger->set_level(debugMode ? spdlog::level::debug : spdlog::level::warn);
    
    // 注册并设置为默认日志记录器
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    
    spdlog::info("日志系统初始化完成");
}

int main(int argc, char* argv[]) {
    std::string inputFileName;
    std::string outputFileName;
    bool debugMode = false;
    
    // 解析参数
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-i" && i + 1 < argc) {
            inputFileName = argv[++i];
        } else if ((arg == "-o") && i + 1 < argc) {
            outputFileName = argv[++i];
        } else if (arg == "-d") {
            debugMode = true;
        } else {
            // 在初始化logger之前不能使用spdlog
            std::cerr << "错误: 无效的参数 '" << arg << "'\n";
            printUsage();
            return 1;
        }
    }

    // 创建日志目录
    std::filesystem::path logDirPath;
    std::string logFileName;
    if (!inputFileName.empty()) {
        std::filesystem::path inputPath(inputFileName);
        logDirPath = "logs" / inputPath.stem();
        std::filesystem::create_directories(logDirPath);
        logFileName = logDirPath / "compile.log";
    } else {
        logDirPath = "logs";
        logFileName = logDirPath / "pascc.log";
    }
    std::filesystem::create_directories(logDirPath);
    
    // 初始化日志系统，传入调试模式标志
    initLogger(logFileName, debugMode);
    
    // 检查必需的输入文件参数
    if (inputFileName.empty()) {
        inputFileName = "./test.pas";
        spdlog::info("使用默认输入文件: '{}'", inputFileName);
    }
    
    // 检查输入文件是否存在
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        spdlog::error("错误: 无法打开输入文件 '{}'", inputFileName);
        return 1;
    }
    
    // 如果没有指定输出文件，则根据输入文件名生成
    if (outputFileName.empty()) {
        if (inputFileName == "./test.pas") {
            outputFileName = "./res.c";
        } else {
            std::filesystem::path inputPath(inputFileName);
            outputFileName = inputPath.stem().string() + ".c";
        }
    }
    
    std::ofstream outputFile(outputFileName);
    if (!outputFile) {
        spdlog::error("错误: 无法创建输出文件 '{}'", outputFileName);
        return 1;
    }
    
    // **1️⃣ 读取 Pascal 源代码**
    spdlog::debug("读取 Pascal 源代码");
    std::ostringstream oss; 
    oss << inputFile.rdbuf();
    auto pascalCode = oss.str();
    inputFile.close();



    // **2️⃣ 调用 Pascal 解析器**
    PascalParser parser;
    if (!parser.parse(pascalCode)) {
        spdlog::error("解析失败: {}", parser.getResult());
        return 1;
    }

    try{

        auto root = std::dynamic_pointer_cast<ProgramAST>(getAST());
        if (root) {
            root->printAST();
            
            auto cCode = std::make_unique<CCodeGenVisitor>();
            root->accept(*cCode);
            
            auto llvmIr = std::make_unique<LlvmVisitor>();
            root->accept(*llvmIr);

            // 写入主输出文件
            outputFile << cCode->output.str() << std::endl;
            
            // 写入 LLVM IR 文件
            std::ofstream llvmIrFile("output.ll");
            llvmIrFile << llvmIr->getIR();
        }
        
        spdlog::info("转换完成: '{}' -> '{}'", inputFileName, outputFileName);
        return 0;
    }catch(const std::bad_cast& e){
        spdlog::error("错误: 无法将 AST 转换为 ProgramAST");
        return 1;
    }catch(const std::exception& e){
        spdlog::error("错误: {}", e.what());
        return 1;
    }



    
    

}

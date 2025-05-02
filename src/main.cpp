#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <string>
#include <filesystem>
#include "Core/PascalParse.h"
#include "AST/ASTBase.h"  // 包含 AST 基类

void printUsage() {
    std::cout << "用法: pascc -i <pascal源文件> [-o <输出文件路径>]" << std::endl;
    std::cout << "示例: pascc -i example.pas -o output.c" << std::endl;
}

void initLogger() {
    // 设置日志格式
    spdlog::set_pattern("[%^%l%$] [%s:%#] %v");
    
    // 设置日志级别
    spdlog::set_level(spdlog::level::debug);
    
    // 创建文件日志并设置为默认日志器
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/pascc.log", true);
    auto logger = std::make_shared<spdlog::logger>("pascc", file_sink);
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    
    spdlog::info("日志系统初始化完成");
}

int main(int argc, char* argv[]) {
    // 解析命令行参数
    if (argc < 3) {
        printUsage();
        return 1;
    }
    
    std::string inputFileName;
    std::string outputFileName;
    initLogger();

    // 解析参数
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-i" && i + 1 < argc) {
            inputFileName = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputFileName = argv[++i];
        } else {
            spdlog::error("错误: 无效的参数 '{}'", arg);
            printUsage();
            return 1;
        }
    }
    
    // 检查必需的输入文件参数
    if (inputFileName.empty()) {
        spdlog::error("错误: 必须指定输入文件");
        printUsage();
        return 1;
    }
    
    // 检查输入文件是否存在
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        spdlog::error("错误: 无法打开输入文件 '{}'", inputFileName);
        return 1;
    }
    
    // 如果没有指定输出文件，则根据输入文件名生成
    if (outputFileName.empty()) {
        std::filesystem::path inputPath(inputFileName);
        outputFileName = inputPath.stem().string() + ".c";
    }
    
    std::ofstream outputFile(outputFileName);
    if (!outputFile) {
        spdlog::error("错误: 无法创建输出文件 '{}'", outputFileName);
        return 1;
    }
    
    // **1️⃣ 读取 Pascal 源代码**
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
            
            outputFile << cCode->output.str() << std::endl;
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

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>
#include "Core/PascalParse.h"
#include "AST/ASTBase.h"  // 包含 AST 基类

void printUsage() {
    std::cout << "用法: pascc -i <pascal源文件>" << std::endl;
    std::cout << "示例: pascc -i example.pas" << std::endl;
}

int main(int argc, char* argv[]) {
    // 解析命令行参数
    if (argc != 3 || std::string(argv[1]) != "-i") {
        printUsage();
        return 1;
    }
    
    std::string inputFileName = argv[2];
    
    // 检查输入文件是否存在
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "错误: 无法打开输入文件 '" << inputFileName << "'" << std::endl;
        return 1;
    }
    
    // 获取输出文件名（将.pas替换为.c，保持文件名相同）
    std::filesystem::path inputPath(inputFileName);
    std::string outputFileName = inputPath.stem().string() + ".c";
    
    std::ofstream outputFile(outputFileName);
    if (!outputFile) {
        std::cerr << "错误: 无法创建输出文件 '" << outputFileName << "'" << std::endl;
        return 1;
    }
    
    // **1️⃣ 读取 Pascal 源代码**
    std::ostringstream oss; 
    oss << inputFile.rdbuf();
    auto pascalCode = oss.str();
    inputFile.close();

    std::cout << "===Pascal Code===\n" << pascalCode << "===============\n\n";
    
    // **2️⃣ 调用 Pascal 解析器**
    PascalParser parser;
    if (!parser.parse(pascalCode)) {
        std::cerr << "解析失败: " << parser.getResult() << std::endl;
        return 1;
    }
    auto& root = getAST();

    // **4️⃣ 输出转换后的 C 代码**
    outputFile << root.codeGen() << std::endl;
    
    std::cout << "转换完成: '" << inputFileName << "' -> '" << outputFileName << "'" << std::endl;

    return 0;
}

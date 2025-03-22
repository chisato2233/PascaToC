#include <iostream>
#include <fstream>
#include <memory>
#include "Core/PascalParse.h"
#include "AST/ASTBase.h"  // 包含 AST 基类

int main(int argc, char* argv[]) {

    // **1️⃣ 读取 Pascal 源代码**
    std::ifstream inputFile("../test.pas",std::ios::binary);
    std::ofstream outputFile("../res.c");

    std::ostringstream oss; 
    oss << inputFile.rdbuf();
    auto pascalCode = oss.str();
    inputFile.close();

    std::cout<<"===Pascal Code===\n"<<pascalCode<<"===============\n\n";
    // **2️⃣ 调用 Pascal 解析器**
    PascalParser parser;
    if (!parser.parse(pascalCode)) {
        std::cerr << "Parsing failed."<<parser.getResult() << std::endl;
        return 1;
    }
    auto& root = getAST();

    // **4️⃣ 输出转换后的 C 代码**
    outputFile << root.codeGen() << std::endl;

    return 0;
}

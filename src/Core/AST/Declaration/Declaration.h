#pragma once
#include "ASTBase.h"
#include <unordered_map>
#include <stack>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include "Expression/Expression.h"
#include "SymbolTable.h"

// 变量声明节点（支持普通变量和数组变量）
// 如果 typeName 含有 '[' 则认为是数组类型，例如 "int[10]"
class VarDeclaration : public ASTAcceptImpl<VarDeclaration,Declaration> {
public:
    std::vector<std::string> varNames;
    std::string typeName; // 可能为 "int" 或形如 "int[10]"
    bool isRef = false;
    VarDeclaration(std::vector<std::string> names, std::string type, bool ref = false)
        : varNames(std::move(names)), typeName(std::move(type)), isRef(ref) {
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Variable Declaration: {}", padding, typeName);
        for (const auto& name : varNames) {
            spdlog::info("{}  - {}", padding, name);
        }
    }
};

_VisitDecl_(CCodeGenVisitor, VarDeclaration) {
    size_t pos = node.typeName.find('[');
    if (pos != std::string::npos) {
        std::string baseType = node.typeName.substr(0, pos);
        std::string arraySuffix = node.typeName.substr(pos);
        for (const auto &name : node.varNames) {
            GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(name, node.typeName));
            output << baseType << " " << name << arraySuffix << ";\n";
        }
    } else {
        for (const auto &name : node.varNames) {
            GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(name, node.typeName, node.isRef));
            output << node.typeName << " " << name << ";\n";
        }
    }
}

#include "FunctionDeclaration.h"
#include "ConstDeclaration.h"
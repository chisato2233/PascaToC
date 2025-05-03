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
    
    VarDeclaration(std::vector<std::string> names, std::string type)
        : varNames(std::move(names)), typeName(std::move(type)) {
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
            GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(name, node.typeName));
            output << node.typeName << " " << name << ";\n";
        }
    }
}

// 常量声明节点
class ConstDeclaration : public ASTAcceptImpl<ConstDeclaration,Declaration> {
public:
    std::string constName;
    ExprPtr constValue;  // 直接存储表达式，而不是字符串值
    
    ConstDeclaration(const std::string& name, ExprPtr value)
        : constName(name), constValue(std::move(value)) {
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Constant Declaration: {}", padding, constName);
        constValue->printAST(indent + 1);
    }
};

_VisitDecl_(CCodeGenVisitor, ConstDeclaration) {
    // 获取表达式的C类型
    std::string c_type = node.constValue->getCType();
    bool is_string = node.constValue->getType() == Expression::ExprType::String;
    
    // 将常量添加到符号表
    auto constInfo = std::make_shared<ConstantInfo>(
        node.constName,
        c_type,
        node.constValue
    );
    
    if (GlobalSymbolTable.addSymbol(constInfo)) {
        spdlog::debug("常量 {} 已添加到符号表，类型: {}", node.constName, c_type);
    }
    
    // 生成C代码
    if (is_string) {
        // 字符串常量使用数组
        output << "static const " << c_type << " " << node.constName << "[] = ";
        node.constValue->accept(*this);
        output << ";\n";
    } else {
        // 其他类型使用const变量
        output << "const " << c_type << " " << node.constName << " = ";
        
        // 布尔值特殊处理
        if (node.constValue->getType() == Expression::ExprType::Boolean) {
            if (auto boolExpr = std::dynamic_pointer_cast<BoolExpr>(node.constValue)) {
                output << (boolExpr->value ? "1" : "0") << ";\n";
                return;
            }
        }
        
        // 生成值
        node.constValue->accept(*this);
        output << ";\n";
    }
}

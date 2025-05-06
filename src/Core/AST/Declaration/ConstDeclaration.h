#pragma once

#include "ASTBase.h"
#include "Expression/Expression.h"
#include "SymbolTable.h"
#include "spdlog/spdlog.h"

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
        c_type
    );
    
    if (GlobalSymbolTable.addSymbol(constInfo)) {
        spdlog::debug("常量 {} 已添加到符号表，类型: {}", node.constName, c_type);
    }
    
    // 生成C代码
    if (is_string) {
        // 字符串常量使用数组
        output << "static const " << c_type << " " << node.constName << " = ";
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

_VisitDecl_(LlvmVisitor, ConstDeclaration) {
    // 先访问常量表达式以获取其LLVM值
    node.constValue->accept(*this);
    llvm::Value* constValue = getLastValue();
    if (!constValue) {
        spdlog::error("Failed to evaluate constant expression for {}", node.constName);
        return;
    }

    // 获取常量的类型
    llvm::Type* constType = constValue->getType();
    
    // 字符串常量需要特殊处理
    // 创建全局字符串常量
    llvm::Constant* strConstant = llvm::cast<llvm::Constant>(constValue);
    auto* globalStr = new llvm::GlobalVariable(
        *module,                                    // 所属模块
        strConstant->getType(),                    // 类型
        true,                                      // 是常量
        llvm::GlobalValue::PrivateLinkage,         // 链接属性
        strConstant,                               // 初始值
        node.constName                             // 名称
    );
    
    // 记录类型信息
    setLLVMValueInfo(node.constName, globalStr, constType, false);
    
}


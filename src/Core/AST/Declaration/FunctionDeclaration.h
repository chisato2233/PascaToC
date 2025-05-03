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


// 函数声明节点
class FunctionDeclaration : public ASTAcceptImpl<FunctionDeclaration, Declaration> {
public:
    std::string funcName;                       // 函数名
    std::string returnType;                     // 返回类型
    std::vector<ParameterInfo> parameters;      // 参数列表
    std::shared_ptr<Statement> body;            // 函数体

    FunctionDeclaration(const std::string& name, std::string returnType,
                       std::vector<ParameterInfo> params, std::shared_ptr<Statement> body)
        : funcName(name), returnType(std::move(returnType)), 
          parameters(std::move(params)), body(std::move(body)) {
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Function Declaration: {} -> {}", padding, funcName, returnType);
        
        if (!parameters.empty()) {
            spdlog::info("{}  Parameters:", padding);
            for (const auto& param : parameters) {
                std::string paramType = param.isRef ? "var " + param.type : param.type;
                spdlog::info("{}    - {}: {}", padding, param.name, paramType);
            }
        }
        
        if (body) {
            spdlog::info("{}  Body:", padding);
            body->printAST(indent + 2);
        }
    }
};

_VisitDecl_(CCodeGenVisitor, FunctionDeclaration) {
    // 添加函数到符号表
    CCodeGenVisitor::current_function_name = node.funcName;
    auto funcInfo = std::make_shared<FunctionInfo>(node.funcName, node.returnType);
    for (const auto& param : node.parameters) {
        funcInfo->addParameter(param.name, param.type, param.isRef);
    }    
    // 生成函数声明
    output << node.returnType << " " << node.funcName << "(";
    
    // 生成参数列表
    for (size_t i = 0; i < node.parameters.size(); ++i) {
        const auto& param = node.parameters[i];
        
        // 处理引用参数（在C中使用指针）
        if (param.isRef) {
            output << param.type << "* " << param.name;
        } else {
            output << param.type << " " << param.name;
        }
        
        if (i < node.parameters.size() - 1) {
            output << ", ";
        }
    }
    output << ") ";
    GlobalSymbolTable.addSymbol(funcInfo);
    // 生成函数体
    // 先进入新的作用域
    GlobalSymbolTable.enterScope();
    
    // 添加参数到符号表
    for (const auto& param : node.parameters) {
        GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(
            param.name, param.isRef ? param.type + "*" : param.type, param.isRef));
    }
    
    // 生成函数体代码
    if (node.body) {
        node.body->accept(*this);
    } else {
        output << "{\n  // Empty function body\n}\n";
    }
    
    // 退出作用域
    GlobalSymbolTable.exitScope();
    CCodeGenVisitor::current_function_name = "";
}

// 过程声明节点
class ProcedureDeclaration : public ASTAcceptImpl<ProcedureDeclaration, Declaration> {
public:
    FunctionDeclaration actual_decl;

    ProcedureDeclaration(const std::string& name, std::vector<ParameterInfo> params, 
                         std::shared_ptr<Statement> body)
            : actual_decl(name, "void",std::move(params), std::move(body)) {
    }

    void printAST(int indent = 0) const override {
        actual_decl.printAST(indent);
    }
};

_VisitDecl_(CCodeGenVisitor, ProcedureDeclaration) {
    node.actual_decl.accept(*this);
}
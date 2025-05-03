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

// 函数参数类，用于表示函数的单个参数
class ParameterInfo {
public:
    std::string name;     // 参数名
    std::string type;     // 参数类型
    bool isRef;           // 是否为引用参数（Pascal中的var参数）

    ParameterInfo(std::string name, std::string type, bool isRef = false)
        : name(std::move(name)), type(std::move(type)), isRef(isRef) {}
};

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
        funcInfo->parameters.push_back({param.name, param.type});
    }
    GlobalSymbolTable.addSymbol(funcInfo);
    
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
    
    // 生成函数体
    // 先进入新的作用域
    GlobalSymbolTable.enterScope();
    
    // 添加参数到符号表
    for (const auto& param : node.parameters) {
        GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(
            param.name, param.isRef ? param.type + "*" : param.type));
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
    std::string procName;                        // 过程名
    std::vector<ParameterInfo> parameters;       // 参数列表
    std::shared_ptr<Statement> body;             // 过程体

    ProcedureDeclaration(const std::string& name, std::vector<ParameterInfo> params, 
                         std::shared_ptr<Statement> body)
        : procName(name), parameters(std::move(params)), body(std::move(body)) {
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Procedure Declaration: {}", padding, procName);
        
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

_VisitDecl_(CCodeGenVisitor, ProcedureDeclaration) {
    CCodeGenVisitor::current_function_name = node.procName;
    // 添加过程到符号表
    auto procInfo = std::make_shared<FunctionInfo>(node.procName, "void");
    for (const auto& param : node.parameters) {
        procInfo->parameters.push_back({param.name, param.type});
    }
    GlobalSymbolTable.addSymbol(procInfo);
    
    // 生成过程声明（在C中是无返回值的函数）
    output << "void " << node.procName << "(";
    
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
    
    // 生成过程体
    // 先进入新的作用域
    GlobalSymbolTable.enterScope();
    
    // 添加参数到符号表
    for (const auto& param : node.parameters) {
        GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(
            param.name, param.isRef ? param.type + "*" : param.type));
    }
    
    // 生成过程体代码
    if (node.body) {
        node.body->accept(*this);
    } else {
        output << "{\n  // Empty procedure body\n}\n";
    }
    
    // 退出作用域
    GlobalSymbolTable.exitScope();
    CCodeGenVisitor::current_function_name = "";
}
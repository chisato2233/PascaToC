#pragma once
#include "ASTBase.h"
#include "Expression/FunctionCall.h"
#include "SymbolTable.h"
#include "spdlog/spdlog.h"


class FunctionCallStmt : public ASTAcceptImpl<FunctionCallStmt,Statement> {
public:
    FunctionCall actual_call;
    FunctionCallStmt(const std::string& function_name, const ExprVec& arguments)
        : actual_call(function_name, arguments) 
        {
        }
    std::string get_name() const noexcept override {return "Function Call Statement";}
    
    void printAST(int indent = 0) const override {
        actual_call.printAST(indent + 1);
    }
};

_VisitDecl_(CCodeGenVisitor,FunctionCallStmt){
    node.actual_call.accept(*this);
    output << ";\n";
}
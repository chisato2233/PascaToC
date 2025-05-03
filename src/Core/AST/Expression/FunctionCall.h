#pragma once
#include "AST/ASTBase.h"
#include "spdlog/spdlog.h"

#include "SymbolTable.h"

class FunctionCall : public ASTAcceptImpl<FunctionCall,Expression> {
public:
    std::string funcName;
    std::vector<std::shared_ptr<Expression>> args;
    FunctionCall(const std::string& name, std::vector<std::shared_ptr<Expression>> args)
        : funcName(name), args(std::move(args)) {
    }

    std::string get_name() const noexcept override {
        return "Function Call [" + funcName + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Function Call: {}", padding, funcName);
        if (!args.empty()) {
            spdlog::info("{}  Arguments:", padding);
            for (const auto& arg : args) {
                arg->printAST(indent + 2);
            }
        }
    }

    ExprType getType() const override {
        auto symbol = GlobalSymbolTable.lookupSymbol(funcName);
        if (!symbol || symbol->symbolType != SymbolType::Function) {
            return ExprType::Unknown;
        }
        
        auto funcInfo = std::dynamic_pointer_cast<FunctionInfo>(symbol);
        if (!funcInfo) return ExprType::Unknown;
        
        if (funcInfo->type == "int") return ExprType::Integer;
        if (funcInfo->type == "double") return ExprType::Real;
        if (funcInfo->type == "char") return ExprType::String;
        return ExprType::Unknown;
    }
};

_VisitDecl_(CCodeGenVisitor,FunctionCall){
    output<< node.funcName << "(";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        if (i + 1 < node.args.size()) 
            output << ", ";
    }
    output << ")";
}


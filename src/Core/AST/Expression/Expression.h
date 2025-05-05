#pragma once
#include "ASTBase.h"
#include "SymbolTable.h"

#include "VariableExpr.h"
#include "BinaryExpression.h"
#include "UnaryExpr.h"


class NumberExpr : public ASTAcceptImpl<NumberExpr,Expression> {

public:
    int value;
    explicit NumberExpr(int val) : value(val) {
    }

    std::string get_name() const noexcept override {
        return "Number [" + std::to_string(value) + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Number: {}", padding, value);
    }

    ExprType getType() const override { return ExprType::Integer; }
};

_VisitDecl_(CCodeGenVisitor,NumberExpr){
    output<<node.value;
}




class RealExpr : public ASTAcceptImpl<RealExpr,Expression> {

public:
    double value;
    explicit RealExpr(double val) : value(val) {
    }

    std::string get_name() const noexcept override {
        return "Real [" + std::to_string(value) + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Real: {}", padding, value);
    }

    ExprType getType() const override { return ExprType::Real; }
};

_VisitDecl_(CCodeGenVisitor,RealExpr){
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.16g", node.value);
    output <<"(double)" "("<<buffer<<")";
}




class BoolExpr : public ASTAcceptImpl<BoolExpr,Expression> {

public:
    bool value;
    explicit BoolExpr(bool val) : value(val) {
    }

    std::string get_name() const noexcept override {
        return "Boolean [" + std::string(value ? "true" : "false") + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Boolean: {}", padding, value ? "true" : "false");
    }

    ExprType getType() const override { return ExprType::Boolean; }
};

_VisitDecl_(CCodeGenVisitor,BoolExpr){
    output<<node.value;
}



class StringExpr : public ASTAcceptImpl<StringExpr,Expression> {

public:
    std::string value;
    explicit StringExpr(const char* val) : value(val) {
    }

    std::string get_name() const noexcept override {
        return "String [" + value + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ String: \"{}\"", padding, value);
    }

    ExprType getType() const override { return ExprType::String; }
};
_VisitDecl_(CCodeGenVisitor,StringExpr){
    output<<"\""<<node.value<<"\"";
}

#include "FunctionCall.h"
#include "ArrayAccessExpr.h"


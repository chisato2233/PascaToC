#pragma once
#include "ASTBase.h"
#include "SymbolTable.h"

#include "VariableExpr.h"
#include "BinaryExpression.h"
#include "UnaryExpr.h"

#include "FunctionCall.h"
#include "ArrayAccessExpr.h"
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

_VisitDecl_(LlvmVisitor,NumberExpr){
    setLastValue(llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(*context),
        node.value,
        true  // isSigned = true
    ));
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

_VisitDecl_(LlvmVisitor,RealExpr){
    setLastValue(llvm::ConstantFP::get(
        llvm::Type::getDoubleTy(*context),
        node.value
    ));
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

_VisitDecl_(LlvmVisitor,BoolExpr){
    setLastValue(llvm::ConstantInt::get(
        llvm::Type::getInt1Ty(*context),
        node.value ? 1 : 0
    ));
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

_VisitDecl_(LlvmVisitor,StringExpr){
    llvm::Constant* strConstant = builder->CreateGlobalStringPtr(
        node.value,
        "str",  // 名称前缀
        0,      // 地址空间
        module.get()
    );
    setLastValue(strConstant);
}




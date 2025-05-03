#pragma once

#include"ASTBase.h"


class WriteStmt : public ASTAcceptImpl<WriteStmt,Statement> {
public:
    ExprVec vec;
    std::string get_name() const noexcept override {return "Write Statement";}
    
    explicit WriteStmt(ExprVec expr)
        : vec(std::move(expr)) {
        }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Write Statement", padding);
        
        for (const auto& expr : vec) {
            expr->printAST(indent + 1);
        }
    }
};

_VisitDecl_(CCodeGenVisitor,WriteStmt){
    for(auto& e : node.vec){
        output<<"write(";
        e->accept(*this);
        output<<");\n";
    }
}



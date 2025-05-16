#pragma once

#include"ASTBase.h"


class WriteStmt : public ASTAcceptImpl<WriteStmt,Statement> {
public:
    ExprVec vec;
    bool isWriteLn;
    std::string get_name() const noexcept override {return "Write Statement";}
    
    explicit WriteStmt(ExprVec expr, bool isWriteLn = false)
        : vec(std::move(expr)), isWriteLn(isWriteLn) {
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
    if(node.isWriteLn){
        for(auto& e : node.vec){
            output<<"write(";
            e->accept(*this);
            output<<");\n";
        }
        output<<"putchar('\\n');\n";
    }else{
        for(auto& e : node.vec){
            output<<"write(";
            e->accept(*this);
            output<<");\n";
        }
    }
}



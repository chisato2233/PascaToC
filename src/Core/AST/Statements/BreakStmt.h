#pragma once

#include "ASTBase.h"
class BreakStmt : public ASTAcceptImpl<BreakStmt,Statement> {
public:
    bool is_break = true;
    BreakStmt()
        {
        }
    std::string get_name() const noexcept override {return "Break Statement";}
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Break", padding);
    }
};

_VisitDecl_(CCodeGenVisitor,BreakStmt){
    if(node.is_break){
        output<<"break;\n";
    }
}

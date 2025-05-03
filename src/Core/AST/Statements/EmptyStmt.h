#pragma once

#include "ASTBase.h"

class EmptyStmt : public ASTAcceptImpl<EmptyStmt,Statement> {
public:
    EmptyStmt() {}
    bool need_coment = true;
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ EmptyStmt", padding);
    }
};

_VisitDecl_(CCodeGenVisitor,EmptyStmt){
    if(node.need_coment){
        output << "/* EmptyStmt */\n";
    }
}

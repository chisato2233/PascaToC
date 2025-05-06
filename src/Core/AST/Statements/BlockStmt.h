#pragma once
#include "ASTBase.h"

class BlockStmt : public ASTAcceptImpl<BlockStmt,Statement> {
private:

public:
    DeclVec declarations;
    StmtPtr body;
    BlockStmt(DeclVec decls, StmtPtr body)
        : declarations(std::move(decls)), body(std::move(body)) {
        }

    std::string get_name() const noexcept override {return "Block Statement";}
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Block", padding);
        
        if (!declarations.empty()) {
            spdlog::info("{}  Declarations:", padding);
            for (const auto& decl : declarations) {
                decl->printAST(indent + 2);
            }
        }
        
        spdlog::info("{}  Body:", padding);
        body->printAST(indent + 2);
    }
};

_VisitDecl_(CCodeGenVisitor,BlockStmt){
    output<<"{\n";
    for(auto& decl : node.declarations){
        decl->accept(*this);
    }
    node.body->accept(*this);
    output<<"}\n";
}

_VisitDecl_(LlvmVisitor,BlockStmt){
    for(auto& decl : node.declarations){
        decl->accept(*this);
    }
    node.body->accept(*this);
}

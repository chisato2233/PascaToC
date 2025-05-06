#pragma once
#include "AST/ASTBase.h"

class CompoundStmt : public ASTAcceptImpl<CompoundStmt,Statement> {
public:
    StmtVec statements;
    std::string get_name() const noexcept override {return "Compound Statement";}
    
    explicit CompoundStmt(StmtVec stmts)
        : statements(std::move(stmts)) {
        }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Compound Statement", padding);
        
        for (size_t i = 0; i < statements.size(); i++) {
            statements[i]->printAST(indent + 1);
        }
    }
};

_VisitDecl_(CCodeGenVisitor,CompoundStmt){
    for(auto& stmt : node.statements){
        stmt->accept(*this);
        output<<"\n";
    }
}

_VisitDecl_(LlvmVisitor,CompoundStmt){
    for(auto& stmt : node.statements){
        stmt->accept(*this);
    }
}


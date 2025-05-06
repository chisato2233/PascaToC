#pragma once

#include "ASTBase.h"

class IfStmt : public ASTAcceptImpl<IfStmt,Statement> {
public:
    
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> thenStmt;
    std::shared_ptr<Statement> elseStmt; // 可能为 nullptr
    
    IfStmt(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> thenS, std::shared_ptr<Statement> elseS = nullptr)
        : condition(std::move(cond)), thenStmt(std::move(thenS)), elseStmt(std::move(elseS)) {
    }

    std::string get_name() const noexcept override {
        return "If Statement";
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ If Statement", padding);
        
        spdlog::info("{}  Condition:", padding);
        condition->printAST(indent + 2);
        
        spdlog::info("{}  Then:", padding);
        thenStmt->printAST(indent + 2);
        
        if (elseStmt) {
            spdlog::info("{}  Else:", padding);
            elseStmt->printAST(indent + 2);
        }
    }
};

_VisitDecl_(CCodeGenVisitor,IfStmt) {
    output <<"if (";
    node.condition->accept(*this);
    output<<"){\n";
    
    node.thenStmt->accept(*this);
    output<<"}\n";
    if(node.elseStmt){
        output<<"else{\n";
        node.elseStmt->accept(*this);
        output<<"}\n";
    }
}

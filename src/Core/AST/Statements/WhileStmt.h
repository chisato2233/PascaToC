#pragma once

#include "ASTBase.h"

class WhileStmt : public ASTAcceptImpl<WhileStmt,Statement> {
public:

    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> body;
    
    WhileStmt(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> body)
        : condition(std::move(cond)), body(std::move(body)) {
    }

    std::string get_name() const noexcept override {
        return "While Statement";
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ While Statement", padding);
        
        spdlog::info("{}  Condition:", padding);
        condition->printAST(indent + 2);
        
        spdlog::info("{}  Body:", padding);
        body->printAST(indent + 2);
    }
};

_VisitDecl_(CCodeGenVisitor, WhileStmt) {
    output << "while (";
    node.condition->accept(*this);  // 使用 accept 而不是 codeGen
    output << "){\n";
    node.body->accept(*this);
    output << "}\n";
}


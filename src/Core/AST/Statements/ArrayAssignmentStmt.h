#pragma once
#include "Statements.h"
#include "Expression/ArrayAccessExpr.h"

// 数组赋值语句
class ArrayAssignmentStmt : public ASTAcceptImpl<ArrayAssignmentStmt, Statement> {
public:
    std::shared_ptr<ArrayAccessExpr> arrayAccess;
    std::shared_ptr<Expression> expr;

    ArrayAssignmentStmt(std::shared_ptr<ArrayAccessExpr> access, std::shared_ptr<Expression> value)
        : arrayAccess(std::move(access)), expr(std::move(value)) {
    }

    std::string get_name() const noexcept override {
        return "Array Assignment Statement";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Array Assignment:", padding);
        arrayAccess->printAST(indent + 1);
        spdlog::info("{}  Value:", padding);
        expr->printAST(indent + 1);
    }
};

_VisitDecl_(CCodeGenVisitor, ArrayAssignmentStmt) {
    // 访问数组元素
    node.arrayAccess->accept(*this);
    
    // 添加赋值操作符
    output << " = ";
    
    // 生成右侧表达式
    node.expr->accept(*this);
    output << ";";
} 
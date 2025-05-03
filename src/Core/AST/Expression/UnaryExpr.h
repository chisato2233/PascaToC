
#pragma once
#include "ASTBase.h"
#include "BinaryExpression.h"
class UnaryExpr : public ASTAcceptImpl<UnaryExpr,Expression> {
public:
    enum Op {
        Not,     // 逻辑非
        Neg,     // 负号 -a
        Pos      // 正号 +a（通常无实际作用）
    };

    UnaryExpr(Op op, std::shared_ptr<Expression> expr)
        : op(op), expr(std::move(expr)) {
    }
    
    std::string get_name() const noexcept override {
        return "Unary Expression [" + get_op_str(op) + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Unary Op: {}", padding, get_op_str(op));
        spdlog::info("{}  Operand:", padding);
        expr->printAST(indent + 2);
    }
    
    std::string get_op_str(Op _op) const {
         std::string opSymbol;
        switch (_op) {
            case Not: opSymbol = "~"; break;
            case Neg: opSymbol = "-"; break;
            case Pos: opSymbol = "+"; break;
            default: opSymbol = "/* unknown unary op */"; break;
        }
        return opSymbol;
    }


    Op op;
    std::shared_ptr<Expression> expr;

    ExprType getType() const override {
        if (op == Not) {
            return ExprType::Boolean;
        }
        return expr->getType();
    }
};

_VisitDecl_(CCodeGenVisitor, UnaryExpr) {
    // 判断是否需要括号
    bool needParentheses = false;
    
    // 如果子表达式是二元表达式，通常需要加括号
    if (std::dynamic_pointer_cast<BinaryExpr>(node.expr)) {
        needParentheses = true;
    }
    
    output << node.get_op_str(node.op) <<' ';
    
    if (needParentheses) {
        output << "(";
    }
    
    node.expr->accept(*this);
    
    if (needParentheses) {
        output << ")";
    }
} 
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

_VisitDecl_(LlvmVisitor, UnaryExpr) {
    // 先访问操作数
    node.expr->accept(*this);
    llvm::Value* operand = getLastValue();
    if (!operand) {
        spdlog::error("Invalid operand in unary expression");
        return;
    }

    llvm::Value* result = nullptr;
    switch (node.op) {
    case UnaryExpr::Not: {
        if (operand->getType()->isIntegerTy(1)) {
            // 布尔值的 NOT
            result = builder->CreateNot(operand, "not");
        } else {
            spdlog::error("NOT operation requires boolean operand");
            return;
        }
        break;
    }
    case UnaryExpr::Neg: {
        if (operand->getType()->isIntegerTy()) {
            // 整数的负号
            result = builder->CreateNeg(operand, "neg");
        } else if (operand->getType()->isDoubleTy()) {
            // 浮点数的负号
            result = builder->CreateFNeg(operand, "fneg");
        } else {
            spdlog::error("Negation requires numeric operand");
            return;
        }
        break;
    }
    case UnaryExpr::Pos: {
        // 正号不需要任何操作，直接使用操作数
        result = operand;
        break;
    }
    default:
        spdlog::error("Unknown unary operator");
        return;
    }

    setLastValue(result);
    setLastValueType(operand->getType());
} 
#pragma once

#include "ASTBase.h"

class BinaryExpr : public ASTAcceptImpl<BinaryExpr,Expression> {
public:
    enum Op {
        Plus, Minus, Multiply, Divide,         // 算术运算
        Equal, NotEqual, Less, LessEqual,      // 比较运算
        Greater, GreaterEqual,                 // 比较运算
        And, Or,                               // 逻辑运算
        Div, Mod,                              // Pascal 整数除法 和 取模
    };

    BinaryExpr(Op op, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {
    }

    std::string get_name() const noexcept override {
        return "Binary Expression [" + get_op_str(op) + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Binary Op: {}", padding, get_op_str(op));
        spdlog::info("{}  Left:", padding);
        lhs->printAST(indent + 2);
        spdlog::info("{}  Right:", padding);
        rhs->printAST(indent + 2);
    }

    std::string get_op_str(Op _op) const {
        std::string opSymbol;
        switch (_op) {
            case Plus: opSymbol = "+"; break;
            case Minus: opSymbol = "-"; break;
            case Multiply: opSymbol = "*"; break;
            case Divide: opSymbol = "/"; break;

            case Equal: opSymbol = "=="; break;
            case NotEqual: opSymbol = "!="; break;
            case Less: opSymbol = "<"; break;
            case LessEqual: opSymbol = "<="; break;
            case Greater: opSymbol = ">"; break;
            case GreaterEqual: opSymbol = ">="; break;


            case And: opSymbol = "&&"; break;
            case Or: opSymbol = "||"; break;
            
            case Div: opSymbol = "/"; break;         // Pascal 的整除在 C++ 也写作 `/`，但需保证两边是整数
            case Mod: opSymbol = "%"; break;
            
            default: opSymbol = "/* unknown_op */"; break;
        }
        return opSymbol;
    }

    // 添加获取操作符优先级的辅助函数
    static int getOperatorPriority(BinaryExpr::Op op) {
        switch (op) {
            // 乘法、除法和模运算优先级最高
            case BinaryExpr::Multiply:
            case BinaryExpr::Divide:
            case BinaryExpr::Div:
            case BinaryExpr::Mod:
                return 5;
                
            // 加法和减法优先级次之
            case BinaryExpr::Plus:
            case BinaryExpr::Minus:
                return 4;
                
            // 比较运算符
            case BinaryExpr::Less:
            case BinaryExpr::LessEqual:
            case BinaryExpr::Greater:
            case BinaryExpr::GreaterEqual:
                return 3;
                
            // 相等性运算符
            case BinaryExpr::Equal:
            case BinaryExpr::NotEqual:
                return 2;
                
            // 逻辑AND
            case BinaryExpr::And:
                return 1;
                
            // 逻辑OR优先级最低
            case BinaryExpr::Or:
                return 0;
                
            default:
                return 0;
        }
    }




    Op op;
    std::shared_ptr<Expression> lhs, rhs;

    ExprType getType() const override {
        // 逻辑运算返回布尔值
        if (op == And || op == Or || 
            op == Equal || op == NotEqual || 
            op == Less || op == LessEqual || 
            op == Greater || op == GreaterEqual) {
            return ExprType::Boolean;
        }
        
        // 数值运算继承左操作数类型，如果左操作数是Real，结果是Real
        if (lhs->getType() == ExprType::Real || rhs->getType() == ExprType::Real) {
            return ExprType::Real;
        }
        
        return lhs->getType();
    }
};

_VisitDecl_(CCodeGenVisitor, BinaryExpr) {
    // 获取当前表达式和子表达式的优先级
    int currentPriority = BinaryExpr::getOperatorPriority(node.op);
    
    // 是否需要括号的标志
    bool needParentheses = false;
    
    // 如果是除法且两个操作数都是整数，则需要强制转换
    bool needCast = (node.op == BinaryExpr::Divide && 
                    node.lhs->getType() == Expression::ExprType::Integer && 
                    node.rhs->getType() == Expression::ExprType::Integer);
    
    // 特殊处理Pascal的mod运算
    if (node.op == BinaryExpr::Mod) {
        // 使用定义的mod函数
        output << "mod(";
        node.lhs->accept(*this);
        output << ", ";
        node.rhs->accept(*this);
        output << ")";
        return;
    }
    
    if (needParentheses) {
        output << "(";
    }
    
    // 左操作数
    if (auto lhsBinary = std::dynamic_pointer_cast<BinaryExpr>(node.lhs)) {
        int lhsPriority = BinaryExpr::getOperatorPriority(lhsBinary->op);
        // 只有当左操作数优先级低于当前操作符时才加括号
        if (lhsPriority < currentPriority) {
            output << "(";
            node.lhs->accept(*this);
            output << ")";
        } else {
            node.lhs->accept(*this);
        }
    } else {
        if (needCast) {
            output << "(double)";
        }
        node.lhs->accept(*this);
    }
    
    // 操作符
    output << " " << node.get_op_str(node.op) << " ";
    
    // 右操作数
    if (auto rhsBinary = std::dynamic_pointer_cast<BinaryExpr>(node.rhs)) {
        int rhsPriority = BinaryExpr::getOperatorPriority(rhsBinary->op);
        // 只有当右操作数优先级低于或等于当前操作符时才加括号
        // (对于非交换操作符如减法和除法，右操作数即使优先级相同也需要括号)
        bool needRightParen = rhsPriority < currentPriority || 
                             (rhsPriority == currentPriority && 
                              (node.op == BinaryExpr::Minus || 
                               node.op == BinaryExpr::Divide || 
                               node.op == BinaryExpr::Div || 
                               node.op == BinaryExpr::Mod));
        
        if (needRightParen) {
            output << "(";
            node.rhs->accept(*this);
            output << ")";
        } else {
            node.rhs->accept(*this);
        }
    } else {
        node.rhs->accept(*this);
    }
    
    if (needParentheses) {
        output << ")";
    }
}


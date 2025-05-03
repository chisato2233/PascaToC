#pragma once
#include "ASTBase.h"
#include "SymbolTable.h"

// **变量引用表达式**
class VariableExpr : public ASTAcceptImpl<VariableExpr,Expression> {
public:
    std::string name;
    explicit VariableExpr(const std::string& name) : name(name) {}
    std::string get_name() const noexcept override {return "Variable Expression [" + name +"]";}
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Variable: {}", padding, name);
    }

    ExprType getType() const override {
        auto symbol = GlobalSymbolTable.lookupSymbol(name);
        if (!symbol) return ExprType::Unknown;
        
        if (symbol->type == "int") return ExprType::Integer;
        if (symbol->type == "double" || symbol->type == "float") return ExprType::Real;
        if (symbol->type == "char") return ExprType::String;
        if (symbol->type == "bool") return ExprType::Boolean;
        // 其他类型处理...
        
        return ExprType::Unknown;
    }
};

_VisitDecl_(CCodeGenVisitor,VariableExpr){
    // 查找符号表
    auto symbol = GlobalSymbolTable.lookupSymbol(node.name);
    
    if (symbol && symbol->symbolType == SymbolType::Function) {
        // 如果是函数，并且不是在赋值语句的左侧
        if (!isLeftSideOfAssignment) {
            // 生成函数调用
            output << node.name << "()";
        } else {
            // 在赋值语句左侧，直接输出名称
            output << node.name;
        }
    } else {
        // 普通变量
        output << node.name;
    }
}



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


class NumberExpr : public ASTAcceptImpl<NumberExpr,Expression> {

public:
    int value;
    explicit NumberExpr(int val) : value(val) {
    }

    std::string get_name() const noexcept override {
        return "Number [" + std::to_string(value) + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Number: {}", padding, value);
    }

    ExprType getType() const override { return ExprType::Integer; }
};

_VisitDecl_(CCodeGenVisitor,NumberExpr){
    output<<node.value;
}




class RealExpr : public ASTAcceptImpl<RealExpr,Expression> {

public:
    double value;
    explicit RealExpr(double val) : value(val) {
    }

    std::string get_name() const noexcept override {
        return "Real [" + std::to_string(value) + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Real: {}", padding, value);
    }

    ExprType getType() const override { return ExprType::Real; }
};
_VisitDecl_(CCodeGenVisitor,RealExpr){
    output<<node.value;
}




class BoolExpr : public ASTAcceptImpl<BoolExpr,Expression> {

public:
    bool value;
    explicit BoolExpr(bool val) : value(val) {
    }

    std::string get_name() const noexcept override {
        return "Boolean [" + std::string(value ? "true" : "false") + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Boolean: {}", padding, value ? "true" : "false");
    }

    ExprType getType() const override { return ExprType::Boolean; }
};

_VisitDecl_(CCodeGenVisitor,BoolExpr){
    output<<node.value;
}



class StringExpr : public ASTAcceptImpl<StringExpr,Expression> {

public:
    std::string value;
    explicit StringExpr(const char* val) : value(val) {
    }

    std::string get_name() const noexcept override {
        return "String [" + value + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ String: \"{}\"", padding, value);
    }

    ExprType getType() const override { return ExprType::String; }
};
_VisitDecl_(CCodeGenVisitor,StringExpr){
    output<<"\""<<node.value<<"\"";
}

#include "FunctionCall.h"
#include "ArrayAccessExpr.h"


#pragma once
#include "ASTBase.h"

// **变量引用表达式**
class VariableExpr : public Expression {
public:
    std::string name;
    explicit VariableExpr(const std::string& name) : name(name) {}
    std::string get_name()override{return "Variable Expression [" + name +"]";}
    std::string codeGen() override {
        return name; // 变量名在 C 代码中保持不变
    }
};

_VisitDecl_(CCodeGenVisitor,VariableExpr){
    output<<node->name;
}



class BinaryExpr : public Expression {
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
        log("BinaryExpr Construct");
    }

    std::string get_op_str(Op _op){
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

    std::string codeGen() override {
        return "(" + lhs->codeGen() + " " + get_op_str(op) + " " + rhs->codeGen() + ")";
    }

    Op op;
    std::shared_ptr<Expression> lhs, rhs;
};

_VisitDecl_(CCodeGenVisitor,BinaryExpr){
    output<< "(" ;
    node->lhs->accept(*this); 
    output<<" " << node->get_op_str(node->op) << " " ;
    node->rhs->accept(*this); 
    output <<")";
}


class UnaryExpr : public Expression {
public:
    enum Op {
        Not,     // 逻辑非
        Neg,     // 负号 -a
        Pos      // 正号 +a（通常无实际作用）
    };

    UnaryExpr(Op op, std::shared_ptr<Expression> expr)
        : op(op), expr(std::move(expr)) {
        log("UnaryExpr Construct");
    }
    std::string get_op_str(Op _op){
         std::string opSymbol;
        switch (_op) {
            case Not: opSymbol = "!"; break;
            case Neg: opSymbol = "-"; break;
            case Pos: opSymbol = "+"; break;
            default: opSymbol = "/* unknown unary op */"; break;
        }
        return opSymbol;
    }

    std::string codeGen() override {
       
        return "(" + get_op_str(op) + expr->codeGen() + ")";
    }

    Op op;
    std::shared_ptr<Expression> expr;
};

_VisitDecl_(CCodeGenVisitor,UnaryExpr){
    output<< "(" ;
    output<<" " << node->get_op_str(node->op) << " " ;
    node->expr->accept(*this); 
    output <<")";
}



class NumberExpr : public Expression {

public:
    int value;
    explicit NumberExpr(int val) : value(val) {
        log("NumberExpr Construct");
    }

    std::string codeGen() override {
        return std::to_string(value);
    }
};

_VisitDecl_(CCodeGenVisitor,NumberExpr){
    output<<node->value;
}




class RealExpr : public Expression {

public:
    double value;
    explicit RealExpr(double val) : value(val) {
        log("RealExpr Construct");
    }

    std::string codeGen() override {
        return std::to_string(value);
    }
};
_VisitDecl_(CCodeGenVisitor,RealExpr){
    output<<node->value;
}




class BoolExpr : public Expression {

public:
    bool value;
    explicit BoolExpr(bool val) : value(val) {
        log("BoolExpr Construct");
    }

    std::string codeGen() override {
        return value ? "true" : "false";
    }
};

_VisitDecl_(CCodeGenVisitor,BoolExpr){
    output<<node->value;
}



class StringExpr : public Expression {

public:
    std::string value;
    explicit StringExpr(const char* val) : value(val) {
        log("StringExpr Construct");
    }

    std::string codeGen() override {
        return "\"" + value + "\"";  // 生成字符串字面量
    }
};
_VisitDecl_(CCodeGenVisitor,StringExpr){
    output<<"\""<<node->value<<"\"";
}



class FunctionCall : public Expression {

public:
    std::string funcName;
    std::vector<std::shared_ptr<Expression>> args;
    FunctionCall(const std::string& name, std::vector<std::shared_ptr<Expression>> args)
        : funcName(name), args(std::move(args)) {
        log("FunctionCall Construct: " + funcName);
    }

    std::string codeGen() override {
        std::string code = funcName + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            code += args[i]->codeGen();
            if (i + 1 < args.size()) code += ", ";
        }
        code += ")";
        return code;
    }

    std::string get_name() override {
        return "FunctionCall to " + funcName;
    }
};
_VisitDecl_(CCodeGenVisitor,FunctionCall){
    output<< node-> funcName << "(";
    for (size_t i = 0; i < node->args.size(); ++i) {
        node->args[i]->accept(*this);
        if (i + 1 < node->args.size()) 
            output << ", ";
    }
    output << ")";
}


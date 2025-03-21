#pragma once
#include "ASTBase.h"

// **变量引用表达式**
class VariableExpr : public Expression {
    std::string name;
public:
    explicit VariableExpr(const std::string& name) : name(name) {}
    std::string get_name()override{return "Variable Expression [" + name +"]";}
    std::string codeGen() override {
        return name; // 变量名在 C 代码中保持不变
    }
};

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

    std::string codeGen() override {
        std::string opSymbol;
        switch (op) {
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

        return "(" + lhs->codeGen() + " " + opSymbol + " " + rhs->codeGen() + ")";
    }

private:
    Op op;
    std::shared_ptr<Expression> lhs, rhs;
};


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

    std::string codeGen() override {
        std::string opSymbol;
        switch (op) {
            case Not: opSymbol = "!"; break;
            case Neg: opSymbol = "-"; break;
            case Pos: opSymbol = "+"; break;
            default: opSymbol = "/* unknown unary op */"; break;
        }
        return "(" + opSymbol + expr->codeGen() + ")";
    }

private:
    Op op;
    std::shared_ptr<Expression> expr;
};

class NumberExpr : public Expression {
    int value;

public:
    explicit NumberExpr(int val) : value(val) {
        log("NumberExpr Construct");
    }

    std::string codeGen() override {
        return std::to_string(value);
    }
};

class RealExpr : public Expression {
    double value;

public:
    explicit RealExpr(double val) : value(val) {
        log("RealExpr Construct");
    }

    std::string codeGen() override {
        return std::to_string(value);
    }
};
class BoolExpr : public Expression {
    bool value;

public:
    explicit BoolExpr(bool val) : value(val) {
        log("BoolExpr Construct");
    }

    std::string codeGen() override {
        return value ? "true" : "false";
    }
};

class StringExpr : public Expression {
    std::string value;

public:
    explicit StringExpr(const char* val) : value(val) {
        log("StringExpr Construct");
    }

    std::string codeGen() override {
        return "\"" + value + "\"";  // 生成字符串字面量
    }
};

class FunctionCall : public Expression {
    std::string funcName;
    std::vector<std::shared_ptr<Expression>> args;

public:
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

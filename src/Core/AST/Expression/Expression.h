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
        // 其他类型处理...
        
        return ExprType::Unknown;
    }
};

_VisitDecl_(CCodeGenVisitor,VariableExpr){
    output<<node.name;
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
    output << "(";
    node.lhs->accept(*this);
    output << " " << node.get_op_str(node.op) << " ";
    node.rhs->accept(*this);
    output << ")";
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
            case Not: opSymbol = "!"; break;
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

_VisitDecl_(CCodeGenVisitor,UnaryExpr){
    output<< "(" ;
    output<<" " << node.get_op_str(node.op) << " " ;
    node.expr->accept(*this); 
    output <<")";
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



class FunctionCall : public ASTAcceptImpl<FunctionCall,Expression> {

public:
    std::string funcName;
    std::vector<std::shared_ptr<Expression>> args;
    FunctionCall(const std::string& name, std::vector<std::shared_ptr<Expression>> args)
        : funcName(name), args(std::move(args)) {
    }

    std::string get_name() const noexcept override {
        return "Function Call [" + funcName + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Function Call: {}", padding, funcName);
        if (!args.empty()) {
            spdlog::info("{}  Arguments:", padding);
            for (const auto& arg : args) {
                arg->printAST(indent + 2);
            }
        }
    }

    ExprType getType() const override {
        // auto symbol = GlobalSymbolTable.lookupSymbol(funcName);
        // if (!symbol || symbol->symbolType != SymbolType::Function) {
        //     return ExprType::Unknown;
        // }
        
        // auto funcInfo = std::dynamic_pointer_cast<FunctionInfo>(symbol);
        // if (!funcInfo) return ExprType::Unknown;
        
        // if (funcInfo->type == "int") return ExprType::Integer;
        // if (funcInfo->type == "double") return ExprType::Real;
        // if (funcInfo->type == "char") return ExprType::String;
        return ExprType::Unknown;
    }
};
_VisitDecl_(CCodeGenVisitor,FunctionCall){
    output<< node.funcName << "(";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        if (i + 1 < node.args.size()) 
            output << ", ";
    }
    output << ")";
}


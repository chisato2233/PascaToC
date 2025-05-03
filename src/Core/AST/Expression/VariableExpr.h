// **变量引用表达式**
#pragma once
#include "ASTBase.h"
#include "SymbolTable.h"
#include "spdlog/spdlog.h"

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
        
        if (symbol->type == "long long") return ExprType::Integer;
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
            output << node.name << "()";
        } else {
            output << node.name;
        }
    } else {
        if(symbol && symbol->symbolType== SymbolType::Variable && std::static_pointer_cast<VariableInfo>(symbol)->isReference){
            output <<"*"<< node.name;
        }else{
            output << node.name;
        }
    }
}

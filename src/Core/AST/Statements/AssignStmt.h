#pragma once
#include "ASTBase.h"
#include "SymbolTable.h"
// **赋值语句**
class AssignmentStmt : public ASTAcceptImpl<AssignmentStmt,Statement> {
public:
    std::string variable;
    std::shared_ptr<Expression> expr;
    AssignmentStmt(const std::string& var, std::shared_ptr<Expression> expr)
        : variable(var), expr(std::move(expr)) {
        }
    std::string get_name() const noexcept override {return "Assignment Statement";}
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Assignment: {} =", padding, variable);
        expr->printAST(indent + 1);
    }
};

_VisitDecl_(CCodeGenVisitor,AssignmentStmt){
    // 设置上下文信息，表示下一个表达式是在赋值语句左侧

    if(node.variable == CCodeGenVisitor::current_function_name){
        output<<"return ";
        node.expr->accept(*this);
        output << ";";
    }else{
        bool oldValue = isLeftSideOfAssignment;
        isLeftSideOfAssignment = true;
            
        // 检查变量是否为引用
        bool isRef = false;
        auto symbolInfo = GlobalSymbolTable.lookupSymbol(node.variable);
        if (symbolInfo && symbolInfo->symbolType == SymbolType::Variable) {
            auto varInfo = std::static_pointer_cast<VariableInfo>(symbolInfo);
            isRef = varInfo->isReference;
        }
        
        // 如果是引用变量，在C中它实际上是指针类型
        if (isRef) {
            output << "*" << node.variable << " = ";  // 在赋值左侧，需要解引用
        } else {
            output << node.variable << " = ";
        }
        
        // 恢复上下文信息，右侧不再是赋值语句左侧
        isLeftSideOfAssignment = false;
        
        // 生成右侧表达式
        node.expr->accept(*this);
        output << ";";
        
        // 恢复原始上下文
        isLeftSideOfAssignment = oldValue;
    }
}



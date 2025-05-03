#pragma once
#include "AST/ASTBase.h"
#include "spdlog/spdlog.h"
#include "SymbolTable.h"
#include "Expression/Expression.h"
class ReadStmt : public ASTAcceptImpl<ReadStmt, Statement> {
public:
    ExprVec vars;  // 存储要读取的变量表达式（通常是VariableExpr）
    std::string get_name() const noexcept override { return "Read Statement"; }
    
    explicit ReadStmt(ExprVec variables)
        : vars(std::move(variables)) {
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Read Statement", padding);
        
        for (const auto& var : vars) {
            var->printAST(indent + 1);
        }
    }
};

_VisitDecl_(CCodeGenVisitor, ReadStmt) {
    for (auto& var : node.vars) {
        auto varExpr = std::dynamic_pointer_cast<VariableExpr>(var);
        if (varExpr) {
            // 检查变量类型，如果是字符串类型则使用read_string
            if (varExpr->getType() == Expression::ExprType::String) {
                // 假设字符串缓冲区最大长度为256
                output << "read_string(" << varExpr->name << ", 256);\n";
            } else {
                output << "read(&" << varExpr->name << ");\n";
            }
        } else {
            // 通用处理...
            output << "// Warning: 下面的表达式可能不是一个有效的左值\n";
            output << "read(&(";
            var->accept(*this);
            output << "));\n";
        }
    }
}
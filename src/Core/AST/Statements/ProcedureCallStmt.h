#pragma once
#include "AST/ASTBase.h"
#include "spdlog/spdlog.h"
#include "SymbolTable.h"

class ProcedureCallStmt : public ASTAcceptImpl<ProcedureCallStmt, Statement> {
public:
    std::string procName;
    std::vector<std::shared_ptr<Expression>> args;
    
    ProcedureCallStmt(const std::string& name, std::vector<std::shared_ptr<Expression>> args)
        : procName(name), args(std::move(args)) {
    }

    std::string get_name() const noexcept override {
        return "Procedure Call [" + procName + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Procedure Call: {}", padding, procName);
        if (!args.empty()) {
            spdlog::info("{}  Arguments:", padding);
            for (const auto& arg : args) {
                arg->printAST(indent + 2);
            }
        }
    }
};

_VisitDecl_(CCodeGenVisitor, ProcedureCallStmt) {
    output << node.procName << "(";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        if (i + 1 < node.args.size()) 
            output << ", ";
    }
    output << ");\n";
}

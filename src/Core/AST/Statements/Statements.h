#pragma once
#include<string>
#include<memory>
#include<vector>
#include<sstream>
#include"ASTBase.h"

#include"Declaration/FunctionDeclaration.h"


#include "AssignStmt.h"
#include "BlockStmt.h"

class CompoundStmt : public ASTAcceptImpl<CompoundStmt,Statement> {
public:
    StmtVec statements;
    std::string get_name() const noexcept override {return "Compound Statement";}
    
    explicit CompoundStmt(StmtVec stmts)
        : statements(std::move(stmts)) {
        }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Compound Statement", padding);
        
        for (size_t i = 0; i < statements.size(); i++) {
            statements[i]->printAST(indent + 1);
        }
    }
};

_VisitDecl_(CCodeGenVisitor,CompoundStmt){
    for(auto& stmt : node.statements){
        stmt->accept(*this);
        output<<"\n";
    }
}


class IfStmt : public ASTAcceptImpl<IfStmt,Statement> {
public:
    
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> thenStmt;
    std::shared_ptr<Statement> elseStmt; // 可能为 nullptr
    
    IfStmt(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> thenS, std::shared_ptr<Statement> elseS = nullptr)
        : condition(std::move(cond)), thenStmt(std::move(thenS)), elseStmt(std::move(elseS)) {
    }

    std::string get_name() const noexcept override {
        return "If Statement";
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ If Statement", padding);
        
        spdlog::info("{}  Condition:", padding);
        condition->printAST(indent + 2);
        
        spdlog::info("{}  Then:", padding);
        thenStmt->printAST(indent + 2);
        
        if (elseStmt) {
            spdlog::info("{}  Else:", padding);
            elseStmt->printAST(indent + 2);
        }
    }
};

_VisitDecl_(CCodeGenVisitor,IfStmt) {
    output <<"if (";
    node.condition->accept(*this);
    output<<"){\n";
    
    node.thenStmt->accept(*this);
    output<<"}\n";
    if(node.elseStmt){
        output<<"else{\n";
        node.elseStmt->accept(*this);
        output<<"}\n";
    }
}

class WhileStmt : public ASTAcceptImpl<WhileStmt,Statement> {
public:

    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> body;
    
    WhileStmt(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> body)
        : condition(std::move(cond)), body(std::move(body)) {
    }

    std::string get_name() const noexcept override {
        return "While Statement";
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ While Statement", padding);
        
        spdlog::info("{}  Condition:", padding);
        condition->printAST(indent + 2);
        
        spdlog::info("{}  Body:", padding);
        body->printAST(indent + 2);
    }
};

_VisitDecl_(CCodeGenVisitor, WhileStmt) {
    output << "while (";
    node.condition->accept(*this);  // 使用 accept 而不是 codeGen
    output << "){\n";
    node.body->accept(*this);
    output << "}\n";
}



class RepeatStmt : public ASTAcceptImpl<RepeatStmt,Statement> {

public:
    std::vector<std::shared_ptr<Statement>> body;
    std::shared_ptr<Expression> condition;
    RepeatStmt(std::vector<std::shared_ptr<Statement>> stmts, std::shared_ptr<Expression> cond)
        : body(std::move(stmts)), condition(std::move(cond)) {
    }

    std::string get_name() const noexcept override {
        return "Repeat Statement";
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Repeat Statement", padding);
        
        spdlog::info("{}  Body:", padding);
        for (const auto& stmt : body) {
            stmt->printAST(indent + 2);
        }
        
        spdlog::info("{}  Until Condition:", padding);
        condition->printAST(indent + 2);
    }
};

_VisitDecl_(CCodeGenVisitor,RepeatStmt){
    output<<"do{\n";
    for(auto& stmt : node.body){
        stmt->accept(*this);
    }
    output<<"}while(!(";
    node.condition->accept(*this);
    output<<"));\n";
}

class ForStmt : public ASTAcceptImpl<ForStmt,Statement> {

public:
    std::string var;
    std::shared_ptr<Expression> startExpr;
    bool isTo; // true for "to", false for "downto"
    std::shared_ptr<Expression> endExpr;
    std::shared_ptr<Statement> body;
    ForStmt(const std::string& variable, std::shared_ptr<Expression> start, bool toDir,
            std::shared_ptr<Expression> end, std::shared_ptr<Statement> body)
        : var(variable), startExpr(std::move(start)), isTo(toDir), endExpr(std::move(end)), body(std::move(body)) {
    }

    std::string get_name() const noexcept override {
        return "For Statement";
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ For Statement: {} {}", padding, var, 
                    (isTo ? "to" : "downto"));
        
        spdlog::info("{}  Start:", padding);
        startExpr->printAST(indent + 2);
        
        spdlog::info("{}  End:", padding);
        endExpr->printAST(indent + 2);
        
        spdlog::info("{}  Body:", padding);
        body->printAST(indent + 2);
    }
};

_VisitDecl_(CCodeGenVisitor,ForStmt){
    output<<"for ("<<node.var<<" = ";
    node.startExpr->accept(*this);
    output<<"; "<<node.var<<" ";
    if(node.isTo)
        output<<"<=";
    else
        output<<">=";
    node.endExpr->accept(*this);
    output<<"; "<<node.var<<" "<<(node.isTo ? "++" : "--")<<"){\n";
    node.body->accept(*this);
    output<<"}\n";
}

class CaseElementStmt : public ASTAcceptImpl<CaseElementStmt,Statement> {

public:
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> stmt;
    CaseElementStmt(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> stmt)
        : condition(std::move(cond)), stmt(std::move(stmt)) {
    }

    std::string get_name() const noexcept override {
        return "Case Element";
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Case Element", padding);
        
        spdlog::info("{}  Value:", padding);
        condition->printAST(indent + 2);
        
        spdlog::info("{}  Statement:", padding);
        stmt->printAST(indent + 2);
    }
};

_VisitDecl_(CCodeGenVisitor,CaseElementStmt){
    output<<"case ";
    node.condition->accept(*this);
    output<<": ";
    node.stmt->accept(*this);
}
class CaseStmt : public ASTAcceptImpl<CaseStmt,Statement>  {

public:
    std::shared_ptr<Expression> expr;
    std::vector<std::shared_ptr<Statement>> branches;
    CaseStmt(std::shared_ptr<Expression> expr, std::vector<std::shared_ptr<Statement>> branches)
        : expr(std::move(expr)), branches(std::move(branches)) {
    }

    std::string get_name() const noexcept override {
        return "Case Statement";
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Case Statement", padding);
        
        spdlog::info("{}  Expression:", padding);
        expr->printAST(indent + 2);
        
        spdlog::info("{}  Branches:", padding);
        for (const auto& branch : branches) {
            branch->printAST(indent + 2);
        }
    }
};

_VisitDecl_(CCodeGenVisitor,CaseStmt){
    output<<"switch (";
    node.expr->accept(*this);
    output<<"){\n";
    for(auto& branch : node.branches){
        branch->accept(*this);
    }
    output<<"}\n";
}

class ReturnStmt : public ASTAcceptImpl<ReturnStmt, Statement> {
public:
    ExprPtr value;
    
    explicit ReturnStmt(ExprPtr val) : value(std::move(val)) {}
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Return Statement", padding);
        value->printAST(indent + 1);
    }
};

_VisitDecl_(CCodeGenVisitor, ReturnStmt) {
    output << "return ";
    node.value->accept(*this);
    output << ";\n";
}




_VisitDecl_(CCodeGenVisitor, ProgramAST) {
    output << node.environment;
    
    // 提取函数和过程声明到全局作用域
    if (auto blockStmt = std::dynamic_pointer_cast<BlockStmt>(node.body)) {
        std::vector<DeclPtr> globalDeclarations;
        std::vector<DeclPtr> localDeclarations;
        
        for (const auto& decl : blockStmt->declarations) {
            decl->accept(*this);
            output << "\n"; 
        }
        
        // 生成main函数，包含局部变量声明
        output << "int main() {\n";
        // 生成函数体代码
        blockStmt->body->accept(*this);
        output << "}\n";
    } else {
        // 如果body不是BlockStmt，则直接生成
        output << "int main() \n";
        node.body->accept(*this);
        output << "\n";
    }
}



#include "ReadStmt.h"
#include "WriteStmt.h"
#include "ArrayAssignmentStmt.h"
#include "EmptyStmt.h"
#include "FunctionCallStmt.h"
#include "BreakStmt.h"
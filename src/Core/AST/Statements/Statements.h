#pragma once
#include<string>
#include<memory>
#include<vector>
#include<sstream>
#include"ASTBase.h"


// **赋值语句**
class AssignmentStmt : public Statement,_CRTPHelp_(AssignmentStmt) {
public:
    _GenerateAccept_()
    std::string variable;
    std::shared_ptr<Expression> expr;
    AssignmentStmt(const std::string& var, std::shared_ptr<Expression> expr)
        : variable(var), expr(std::move(expr)) {
           log("AssignmentStmt Construct");
        }
    std::string get_name()override {return "Assignment Statement";}
    std::string codeGen() override {
        
        return variable + " = " + expr->codeGen() + ";";
    }
};

_VisitDecl_(CCodeGenVisitor,AssignmentStmt){
    output<< node->variable << " = "; 
    node->expr->accept(*this);
    output << ";";
}




class BlockStmt : public Statement,_CRTPHelp_(BlockStmt) {
private:

public:
    _GenerateAccept_()
    DeclVec declarations;
    StmtPtr body;
    BlockStmt(DeclVec decls, StmtPtr body)
        : declarations(std::move(decls)), body(std::move(body)) {
            log("BlockStmt Construct");
        }

    std::string get_name()override {return "Block Statement";}
    std::string codeGen() override {
        std::ostringstream logstr;
        logstr << "[declaration count = "<<declarations.size()<<"] [body name = "<<body->get_name() <<"]";
        log(logstr.str());


        std::ostringstream out;
        out << "{\n";
        logstr.clear();
        for (const auto& decl : declarations) {
            out << decl->codeGen();
        }
        out << body->codeGen();
        out << "}\n";
        return out.str();
    }
};

_VisitDecl_(CCodeGenVisitor,BlockStmt){
    output<<"{\n";
    for(auto& decl : node->declarations){
        decl->accept(*this);
    }
    node->body->accept(*this);
    output<<"}\n";
}



class CompoundStmt : public Statement,_CRTPHelp_(CompoundStmt) {
private:

public:
    _GenerateAccept_()
    StmtVec statements;
    std::string get_name()override{return "Compound Statement";}
    // 构造函数：接受多个语句
    explicit CompoundStmt(StmtVec stmts)
        : statements(std::move(stmts)) {
            log("CompoundStmt Construt");
        }

    // 生成 C 代码
    std::string codeGen() override {
        std::ostringstream out,size_log;
        size_log << "[Statement Count = "<<statements.size()<<"]";
        log(size_log.str());

        int i = 0;
        for (const auto& stmt : statements) {
            std::ostringstream logstr;
            logstr<<"["<<i<<"] "<<stmt->get_name();
            log(logstr.str());
            i++;
        }
        for (const auto& stmt : statements) {
            out << stmt->codeGen() << "\n";
        }
        return out.str();
    }
};

_VisitDecl_(CCodeGenVisitor,CompoundStmt){
    for(auto& stmt : node->statements){
        stmt->accept(*this);
        output<<"\n";
    }
}

class WriteStmt : public Statement,_CRTPHelp_(WriteStmt) {
public:
    _GenerateAccept_()
    ExprVec vec;
    std::string get_name()override{return "Write Statement";}
    // 构造函数：接受多个语句
    explicit WriteStmt(ExprVec expr)
        : vec(std::move(expr)) {
            log("WriteStmt Construt");
        }

    // 生成 C 代码
    std::string codeGen() override {
        std::ostringstream out,size_log;
        size_log << "[Output expression count  = "<<vec.size()<<"]";
        log(size_log.str());
        for(auto& e : vec){
            out<<"write("<<e->codeGen()<<");\n";
        }
        return out.str();
    }
};

_VisitDecl_(CCodeGenVisitor,WriteStmt){
    output<<"write(";
    for(auto& e : node->vec){
        e->accept(*this);
        if(e != node->vec.back())
            output<<",";
    }
    output<<");\n";
}

class IfStmt : public Statement,_CRTPHelp_(IfStmt) {
public:
    _GenerateAccept_()

    
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> thenStmt;
    std::shared_ptr<Statement> elseStmt; // 可能为 nullptr
    IfStmt(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> thenS, std::shared_ptr<Statement> elseS = nullptr)
        : condition(std::move(cond)), thenStmt(std::move(thenS)), elseStmt(std::move(elseS)) {
        log("IfStmt Construct");
    }

    std::string get_name() override {
        return "If Statement";
    }

    std::string codeGen() override {
        std::string code = "if (" + condition->codeGen() + ") " + thenStmt->codeGen();
        if (elseStmt)
            code += " else " + elseStmt->codeGen();
        return code;
    }
};

_VisitDecl_(CCodeGenVisitor,IfStmt) {
    output<<"if ("<<node->condition->codeGen()<<"){\n";
    node->thenStmt->accept(*this);
    output<<"}\n";
    if(node->elseStmt){
        output<<"else{\n";
        node->elseStmt->accept(*this);
        output<<"}\n";
    }
}

class WhileStmt : public Statement,_CRTPHelp_(WhileStmt) {
public:
    _GenerateAccept_()

    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> body;
    WhileStmt(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> body)
        : condition(std::move(cond)), body(std::move(body)) {
        log("WhileStmt Construct");
    }

    std::string get_name() override {
        return "While Statement";
    }

    std::string codeGen() override {
        return "while (" + condition->codeGen() + ") " + body->codeGen();
    } 
};

_VisitDecl_(CCodeGenVisitor,WhileStmt){
    output<<"while ("<<node->condition->codeGen()<<"){\n";
    node->body->accept(*this);
    output<<"}\n";
}



class RepeatStmt : public Statement,_CRTPHelp_(RepeatStmt) {

public:
    std::vector<std::shared_ptr<Statement>> body;
    std::shared_ptr<Expression> condition;
    _GenerateAccept_()
    RepeatStmt(std::vector<std::shared_ptr<Statement>> stmts, std::shared_ptr<Expression> cond)
        : body(std::move(stmts)), condition(std::move(cond)) {
        log("RepeatStmt Construct");
    }

    std::string get_name() override {
        return "Repeat Statement";
    }

    std::string codeGen() override {
        std::string code = "do {\n";
        for (const auto& stmt : body) {
            code += "  " + stmt->codeGen() + "\n";
        }
        code += "} while (!(" + condition->codeGen() + "));";
        return code;
    }
};

_VisitDecl_(CCodeGenVisitor,RepeatStmt){
    output<<"do{\n";
    for(auto& stmt : node->body){
        stmt->accept(*this);
    }
    output<<"}while(!(";
    node->condition->accept(*this);
    output<<"));\n";
}
class ForStmt : public Statement,_CRTPHelp_(ForStmt) {

public:
    std::string var;
    std::shared_ptr<Expression> startExpr;
    bool isTo; // true for "to", false for "downto"
    std::shared_ptr<Expression> endExpr;
    std::shared_ptr<Statement> body;
    ForStmt(const std::string& variable, std::shared_ptr<Expression> start, bool toDir,
            std::shared_ptr<Expression> end, std::shared_ptr<Statement> body)
        : var(variable), startExpr(std::move(start)), isTo(toDir), endExpr(std::move(end)), body(std::move(body)) {
        log("ForStmt Construct");
    }

    std::string get_name() override {
        return "For Statement";
    }

    std::string codeGen() override {
        std::string op = isTo ? "<=" : ">=";
        std::string step = isTo ? "++" : "--";
        return "for (" + var + " = " + startExpr->codeGen() + "; " +
               var + " " + op + " " + endExpr->codeGen() + "; " +
               var + step + ") " + body->codeGen();
    }
};

_VisitDecl_(CCodeGenVisitor,ForStmt){
    output<<"for ("<<node->var<<" = ";
    node->startExpr->accept(*this);
    output<<"; "<<node->var<<" ";
    if(node->isTo)
        output<<"<=";
    else
        output<<">=";
    output<<node->endExpr->codeGen()<<"; "<<node->var<<" "<<(node->isTo ? "++" : "--")<<"){\n";
    node->body->accept(*this);
    output<<"}\n";
}

class CaseElementStmt : public Statement,_CRTPHelp_(CaseElementStmt) {

public:
    _GenerateAccept_()
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> stmt;
    CaseElementStmt(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> stmt)
        : condition(std::move(cond)), stmt(std::move(stmt)) {
        log("CaseElementStmt Construct");
    }

    std::string get_name() override {
        return "Case Element";
    }

    std::string codeGen() override {
        // 单个分支： case val: ...;
        return "case " + condition->codeGen() + ": " + stmt->codeGen();
    }
};

_VisitDecl_(CCodeGenVisitor,CaseElementStmt){
    output<<"case ";
    node->condition->accept(*this);
    output<<": ";
    node->stmt->accept(*this);
}
class CaseStmt : public Statement,_CRTPHelp_(CaseStmt)  {

public:
    _GenerateAccept_()
    std::shared_ptr<Expression> expr;
    std::vector<std::shared_ptr<Statement>> branches;
    CaseStmt(std::shared_ptr<Expression> expr, std::vector<std::shared_ptr<Statement>> branches)
        : expr(std::move(expr)), branches(std::move(branches)) {
        log("CaseStmt Construct");
    }

    std::string get_name() override {
        return "Case Statement";
    }

    std::string codeGen() override {
        std::string code = "switch (" + expr->codeGen() + ") {\n";
        for (const auto& branch : branches) {
            code += "  " + branch->codeGen() + "\n";
        }
        code += "}";
        return code;
    }
};

_VisitDecl_(CCodeGenVisitor,CaseStmt){
    output<<"switch ("<<node->expr->codeGen()<<"){\n";
    for(auto& branch : node->branches){
        branch->accept(*this);
    }
    output<<"}\n";
}

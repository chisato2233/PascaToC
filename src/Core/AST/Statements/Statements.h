#pragma once
#include<string>
#include<memory>
#include<vector>
#include<sstream>
#include"ASTBase.h"


// **赋值语句**
class AssignmentStmt : public Statement {
    std::string variable;
    std::shared_ptr<Expression> expr;
public:
    AssignmentStmt(const std::string& var, std::shared_ptr<Expression> expr)
        : variable(var), expr(std::move(expr)) {
           log("AssignmentStmt Construct");
        }
    std::string get_name()override {return "Assignment Statement";}
    std::string codeGen() override {
        
        return variable + " = " + expr->codeGen() + ";";
    }
};



class BlockStmt : public Statement {
private:
    DeclVec declarations;
    StmtPtr body;

public:
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

class CompoundStmt : public Statement {
private:
    StmtVec statements;

public:
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

class WriteStmt : public Statement{
    ExprVec vec;
public:
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

class IfStmt : public Statement {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> thenStmt;
    std::shared_ptr<Statement> elseStmt; // 可能为 nullptr

public:
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

class WhileStmt : public Statement {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> body;

public:
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


class RepeatStmt : public Statement {
    std::vector<std::shared_ptr<Statement>> body;
    std::shared_ptr<Expression> condition;

public:
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

class ForStmt : public Statement {
    std::string var;
    std::shared_ptr<Expression> startExpr;
    bool isTo; // true for "to", false for "downto"
    std::shared_ptr<Expression> endExpr;
    std::shared_ptr<Statement> body;

public:
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

class CaseElementStmt : public Statement {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> stmt;

public:
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

class CaseStmt : public Statement {
    std::shared_ptr<Expression> expr;
    std::vector<std::shared_ptr<Statement>> branches;

public:
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

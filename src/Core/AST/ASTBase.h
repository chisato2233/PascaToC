#pragma once
#include <string>
#include<iostream>


// AST 节点基类
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string codeGen() = 0; // 生成 C 代码
    virtual std::string get_name() {return "Defult AST Node";}
    virtual void log(std::string str){
        std::cout<< "["<< get_name()<<"]>>> " << str <<'\n';
    }
};
// 语句节点基类
class Statement : public ASTNode {
public:
    std::string get_name()override{return "Statement";}
    virtual ~Statement() = default;
};
using StmtPtr = std::shared_ptr<Statement>;
using StmtVec = std::vector<StmtPtr>;


// 表达式节点基类
class Expression : public ASTNode {
public:
    std::string get_name()override{return "Expression";}
    virtual ~Expression() = default;
};
using ExprPtr = std::shared_ptr<Expression>;
using ExprVec = std::vector<ExprPtr>;



class Declaration : public ASTNode {
public:
    std::string get_name()override{return "Declaration";}
    virtual ~Declaration() = default;
};
using DeclPtr = std::shared_ptr<Declaration>;
using DeclVec = std::vector<DeclPtr>;

//程序开始结点
class ProgramAST : public ASTNode {
    std::string name;  // Pascal 主程序名称
    std::shared_ptr<Statement> body;  // 程序主体
    std::string environment = 
        #include "environment.h"
    ;

public:
    ProgramAST(const std::string& name, std::shared_ptr<Statement> body)
        : name(name), body(std::move(body)) {
            log("ProgramAST Construct");
        }

    std::string codeGen() override {
        std::ostringstream str;
        str  << "[name = " << name<<"]" << " [body name = "<<body->get_name()<<"]";
        log(str.str());

        std::ostringstream output;
        output << environment;
        output << "int main() \n";
        output << body->codeGen(); // 生成 C 代码
        output << "\n";
        return output.str();
    }
};


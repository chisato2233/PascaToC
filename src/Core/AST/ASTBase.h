#pragma once
#include <string>
#include<iostream>
#include<concepts>
#include"visitor.h"

#define _GenerateAccept_()                                \
    template<typename Visitor>                            \
    void accept(Visitor& visitor) {                       \
        visitor.visit(std::static_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this())); \
    }

#define _CRTPHelp_(type) public std::enable_shared_from_this<type>
#define _VisitDecl_(VisitorType,NodeType)                                              \
        template<>                                                                      \
        inline void VisitorType::visitImpl(std::shared_ptr<NodeType> node)


// AST 节点基类
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string codeGen() = 0; // 生成 C 代码
    virtual std::string get_name() {return "Defult AST Node";}
    
    template<typename Visitor> void accept(Visitor& visitor);

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
struct ProgramAST : public ASTNode ,_CRTPHelp_(ProgramAST){
public:
    std::string name;  // Pascal 主程序名称
    std::shared_ptr<Statement> body;  // 程序主体
    std::string environment = 
        #include "environment.h"
    ;
    _GenerateAccept_()

    ProgramAST(const std::string& name, std::shared_ptr<Statement> body)
        : name(name), body(std::move(body)) {
            log("ProgramAST Construct");
        }

    std::string codeGen() override {
        std::ostringstream str;
        std::ostringstream output;
        str  << "[name = " << name<<"]" << " [body name = "<<body->get_name()<<"]";
        log(str.str());


        output << environment;
        output << "int main() \n";

        output<<body->codeGen();
        
        output << "\n";
        return output.str();
    }
};

_VisitDecl_(CCodeGenVisitor,ProgramAST){
    std::ostringstream str;
    str  << "[name = " << node->name<<"]" << " [body name = "<<node->body->get_name()<<"]";
    node->log(str.str());


    output << node->environment;
    output << "int main() \n";

    node->body->accept(*this);
    
    output << "\n";
};
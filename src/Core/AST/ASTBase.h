#pragma once
#include <string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include"visitor.h"
#include "spdlog/spdlog.h"
#include "environment.h"
/*
NodeBase::accept(VisitorType& visitor) -> [Virtual Function]
AcceptImpl<NodeType>::_acceptBridge(VisitorBase& visBase) ->
dispatch_tuple<AllVisitors>  p == VisitorType* [if constexpr] ->
VisitImplCRTP<VisitorType>::visit<NodeType>(NodeType& node)->
VisitorType::visit<NodeType>(NodeType& node)

          ┌────────────┐
          │ ASTNode* n │   ← 基类指针/ shared_ptr
          └─────┬──────┘
                │ n->accept(visitor)
❶ 非虚模板      │
                ▼
          ┌──────────────────┐
          │ _acceptBridge()  │ ← virtual；节点真实类型决定落点
          └─────┬────────────┘
                │ ❷
                ▼
          ╔══════════════════════════════════╗
          ║ dispatch_tuple<AllVisitors>      ║
          ║   对每个访客做 dynamic_cast      ║
          ║   命中 ⇒ 调 vb.visit(node)      ║
          ║   否则继续…                     ║
          ╚══════════════════════════════════╝
                │ ❸ 命中
                ▼
          ┌──────────────────────────┐
          │ VisitImplCRTP<Vis>::visit│ ← if constexpr 检查
          └────────┬─────────────────┘
                   │ ❹
                   ▼
          ┌──────────────────────────┐
          │   用户自写的             │
          │ Vis::visit(Derived&)     │
          └──────────────────────────┘


一次虚函数调用 + 一次分支（在visitor少的情况下）
*/

/**
 * @brief 为访问者模式定义访问方法的宏
 * @param VisitorType 访问者类型
 * @param NodeType 被访问的节点类型
 * 
 * 使用此宏可以简化访问者模式中具体访问方法的声明
 */
#define _VisitDecl_(VisitorType,NodeType)                                              \
        template<>                                                                      \
        inline void VisitorType::visit(NodeType& node)


/**
 * @brief AST节点的基类
 * 
 * 所有抽象语法树节点都继承自此类，提供了访问者模式的接口和基本功能
 */
class ASTNode {
public:
    /**
     * @brief 虚析构函数
     */
    virtual ~ASTNode() = default;

    /**
     * @brief 生成C代码的方法（已弃用）
     * @return 生成的C代码字符串
     * @deprecated 请使用CCodeGenVisitor代替
     */
    [[deprecated("Use CCodeGenVisitor instead")]]
    virtual std::string codeGen(){return "";} // 生成 C 代码
    
    /**
     * @brief 获取节点名称
     * @return 节点的名称字符串
     */
    virtual std::string get_name() const noexcept {return "Default AST Node";}
    
    /**
     * @brief 接受访问者的方法
     * @tparam Visitor 访问者类型
     * @param visitor 访问者对象
     * @return 访问结果
     * 
     * 实现访问者模式的核心方法，允许不同的访问者访问节点
     */
    template<typename Visitor> 
    auto accept(Visitor&& visitor) ->decltype(auto) {
        
        return _acceptBridge(visitor);
    }

    /**
     * @brief 打印AST结构
     * @param indent 缩进级别
     * 
     * 用于调试和可视化AST结构
     */
    virtual void printAST(int indent = 0) const {
        std::string padding(indent * 2, ' ');
        spdlog::info("{}+ {}", padding, get_name());
    }
    
    /**
     * @brief 获取缩进字符串
     * @param level 缩进级别
     * @return 对应级别的缩进字符串
     */
    std::string getIndent(int level) const {
        return std::string(level * 2, ' ');
    }

protected:
    /**
     * @brief 访问者桥接方法
     * @param visitor 基类访问者
     * 
     * 子类需要实现此方法以支持访问者模式
     */
    virtual void _acceptBridge(ASTVisitorBase& visitor) = 0;
};


/**
 * @brief AST节点接受访问者的实现模板类
 * @tparam Derived 派生类类型
 * @tparam Base 基类类型，默认为ASTNode
 * 
 * 通过CRTP模式实现访问者模式的基础设施
 */
template<typename Derived,typename Base = ASTNode>
class ASTAcceptImpl : public Base{
protected:
    /**
     * @brief 实现访问者桥接方法
     * @param vb 访问者基类引用
     * 
     * 通过dispatch_tuple分发到正确的访问者实现
     */
    void _acceptBridge(ASTVisitorBase& vb) override {
        if(!dispatch_tuple<0, AllVisitors, Derived>(vb, static_cast<Derived&>(*this)))
            vb.defaultVisit(static_cast<Derived&>(*this));
    }
};

/**
 * @brief 语句节点基类
 * 
 * 所有语句类型的AST节点的基类
 */
class Statement : public ASTAcceptImpl<Statement> {
public:
    /**
     * @brief 获取节点名称
     * @return 节点名称字符串
     */
    std::string get_name() const noexcept override {return "Statement";}
    
    /**
     * @brief 虚析构函数
     */
    virtual ~Statement() = default;
};
/**
 * @brief 语句节点的智能指针类型
 */
using StmtPtr = std::shared_ptr<Statement>;
/**
 * @brief 语句节点智能指针的向量类型
 */
using StmtVec = std::vector<StmtPtr>;


/**
 * @brief 表达式节点基类
 * 
 * 所有表达式类型的AST节点的基类
 */
class Expression : public ASTAcceptImpl<Expression> {
public:
    /**
     * @brief 获取节点名称
     * @return 节点名称字符串
     */
    std::string get_name() const noexcept override {return "Expression";}
    
    /**
     * @brief 虚析构函数
     */
    virtual ~Expression() = default;

    enum class ExprType {
        Integer,
        Real,
        Boolean,
        Char,
        String,
        Unknown
    };

    virtual ExprType getType() const { return ExprType::Unknown; }
    virtual std::string getCType() const { 
        switch(getType()) {
            case ExprType::Integer: return "long long";
            case ExprType::Real: return "double";
            case ExprType::Boolean: return "int"; // C中通常用int表示布尔值
            case ExprType::Char: return "char";
            case ExprType::String: return "char*"; // 字符串特殊处理
            default: return "void";
        }
    }
};
/**
 * @brief 表达式节点的智能指针类型
 */
using ExprPtr = std::shared_ptr<Expression>;
/**
 * @brief 表达式节点智能指针的向量类型
 */
using ExprVec = std::vector<ExprPtr>;


/**
 * @brief 声明节点基类
 * 
 * 所有声明类型的AST节点的基类
 */
class Declaration : public ASTAcceptImpl<Declaration> {
public:
    /**
     * @brief 获取节点名称
     * @return 节点名称字符串
     */
    std::string get_name() const noexcept override {return "Declaration";}
    
    /**
     * @brief 虚析构函数
     */
    virtual ~Declaration() = default;
};
/**
 * @brief 声明节点的智能指针类型
 */
using DeclPtr = std::shared_ptr<Declaration>;
/**
 * @brief 声明节点智能指针的向量类型
 */
using DeclVec = std::vector<DeclPtr>;


/**
 * @brief 程序AST根节点
 * 
 * 表示整个Pascal程序的AST根节点
 */
class ProgramAST : public ASTAcceptImpl<ProgramAST>{
public:
    std::string name;  // Pascal 主程序名称
    std::shared_ptr<Statement> body;  // 程序主体
    std::string environment = ENVIRONMENT_H.data();

    /**
     * @brief 构造函数
     * @param name 程序名称
     * @param body 程序主体语句
     */
    ProgramAST(const std::string& name, std::shared_ptr<Statement> body)
        : name(name), body(std::move(body)) 
    {
        
    }

    /**
     * @brief 打印AST结构
     * @param indent 缩进级别
     */
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Program: {}", padding, name);
        if (body) {
            spdlog::info("{}  Body:", padding);
            body->printAST(indent + 1);
        }
    }
};

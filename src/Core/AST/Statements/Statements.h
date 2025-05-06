#pragma once
#include<string>
#include<memory>
#include<vector>
#include<sstream>
#include"ASTBase.h"

#include"Declaration/FunctionDeclaration.h"


#include "AssignStmt.h"
#include "BlockStmt.h"
#include "IfStmt.h"
#include "CompoundStmt.h"
#include "ReadStmt.h"
#include "WhileStmt.h"
#include "WriteStmt.h"
#include "ArrayAssignmentStmt.h"
#include "EmptyStmt.h"
#include "FunctionCallStmt.h"
#include "BreakStmt.h"

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

_VisitDecl_(LlvmVisitor, ForStmt) {
    // 1. 获取循环变量信息
    const auto* loopVarInfo = getLLVMValueInfo(node.var);
    if (!loopVarInfo) {
        spdlog::error("Loop variable not found: {}", node.var);
        return;
    }
    llvm::Value* loopVar = loopVarInfo->value;
    llvm::Type* loopVarType = loopVarInfo->allocatedType;

    // 2. 计算起始值并赋值给循环变量
    node.startExpr->accept(*this);
    llvm::Value* startValue = getLastValue();
    if (!startValue) {
        spdlog::error("Invalid start expression in for loop");
        return;
    }
    builder->CreateStore(startValue, loopVar);
    
    // 3. 获取当前函数
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    
    // 4. 创建必要的基本块
    llvm::BasicBlock* loopCondBlock = llvm::BasicBlock::Create(*context, "loop_cond", function);
    llvm::BasicBlock* loopBodyBlock = llvm::BasicBlock::Create(*context, "loop_body");
    llvm::BasicBlock* loopIncBlock = llvm::BasicBlock::Create(*context, "loop_inc");
    llvm::BasicBlock* afterLoopBlock = llvm::BasicBlock::Create(*context, "after_loop");
    
    // 5. 跳转到条件检查块
    builder->CreateBr(loopCondBlock);
    
    // 6. 生成条件检查代码
    builder->SetInsertPoint(loopCondBlock);
    
    // 加载循环变量的当前值
    llvm::Value* currentValue = builder->CreateLoad(loopVarType, loopVar, "current");
    
    // 计算结束条件
    node.endExpr->accept(*this);
    llvm::Value* endValue = getLastValue();
    if (!endValue) {
        spdlog::error("Invalid end expression in for loop");
        return;
    }
    
    // 根据是to还是downto创建比较
    llvm::Value* condValue;
    if (node.isTo) {
        // to: 检查 current <= end
        condValue = builder->CreateICmpSLE(currentValue, endValue, "loopcond");
    } else {
        // downto: 检查 current >= end
        condValue = builder->CreateICmpSGE(currentValue, endValue, "loopcond");
    }
    
    // 创建条件分支
    builder->CreateCondBr(condValue, loopBodyBlock, afterLoopBlock);
    
    // 7. 生成循环体代码
    loopBodyBlock->insertInto(function);
    builder->SetInsertPoint(loopBodyBlock);
    
    // 执行循环体
    node.body->accept(*this);
    
    // 如果循环体没有终止，跳转到增量块
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(loopIncBlock);
    }
    
    // 8. 生成增量代码
    loopIncBlock->insertInto(function);
    builder->SetInsertPoint(loopIncBlock);
    
    // 加载当前值
    currentValue = builder->CreateLoad(loopVarType, loopVar, "current_inc");
    
    // 根据是to还是downto增加或减少值
    llvm::Value* nextValue;
    if (node.isTo) {
        // to: 增加 current + 1
        nextValue = builder->CreateAdd(
            currentValue, 
            llvm::ConstantInt::get(currentValue->getType(), 1),
            "next"
        );
    } else {
        // downto: 减少 current - 1
        nextValue = builder->CreateSub(
            currentValue, 
            llvm::ConstantInt::get(currentValue->getType(), 1),
            "next"
        );
    }
    
    // 存储新值
    builder->CreateStore(nextValue, loopVar);
    
    // 跳回到条件检查块
    builder->CreateBr(loopCondBlock);
    
    // 9. 设置循环后的插入点
    afterLoopBlock->insertInto(function);
    builder->SetInsertPoint(afterLoopBlock);
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



//#------------------------------------------------------------------------
// Program AST Begin
//#------------------------------------------------------------------------
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


_VisitDecl_(LlvmVisitor, ProgramAST) {

    llvm::FunctionType* mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context),  // 返回类型 int
        false  // 不是可变参数
    );

    llvm::Function* mainFunction = llvm::Function::Create(
        mainType,
        llvm::Function::ExternalLinkage,
        "main",
        module.get()
    );


    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(
        *context,
        "entry",
        mainFunction
    );
    builder->SetInsertPoint(entryBlock);

    node.body->accept(*this);
    if (!builder->GetInsertBlock()->getTerminator()) {
        // 返回0
        builder->CreateRet(llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(*context), 
            0
        ));
    }



}
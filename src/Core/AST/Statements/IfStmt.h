#pragma once

#include "ASTBase.h"

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

_VisitDecl_(LlvmVisitor, IfStmt) {
    // 1. 获取当前函数
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    
    // 2. 创建基本块
    llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock* elseBlock = nullptr;
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context, "ifcont"); // 暂不添加到函数
    
    if (node.elseStmt) {
        elseBlock = llvm::BasicBlock::Create(*context, "else"); // 暂不添加到函数
    }
    
    // 3. 计算条件表达式
    node.condition->accept(*this);
    llvm::Value* condValue = getLastValue();
    if (!condValue) {
        spdlog::error("Invalid condition in if statement");
        return;
    }
    
    // 4. 确保条件是布尔类型
    if (!condValue->getType()->isIntegerTy(1)) {
        condValue = builder->CreateICmpNE(
            condValue, 
            llvm::ConstantInt::get(condValue->getType(), 0),
            "ifcond"
        );
    }
    
    // 5. 创建条件跳转
    if (node.elseStmt) {
        builder->CreateCondBr(condValue, thenBlock, elseBlock);
    } else {
        builder->CreateCondBr(condValue, thenBlock, mergeBlock);
    }
    
    // 6. 生成Then语句的代码
    builder->SetInsertPoint(thenBlock);
    node.thenStmt->accept(*this);
    
    // 如果Then部分没有终止指令，添加跳转到merge块
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBlock);
    }
    
    // 7. 生成Else语句的代码(如果有)
    if (node.elseStmt) {
        // 将else块添加到函数
        elseBlock->insertInto(function);
        builder->SetInsertPoint(elseBlock);
        
        node.elseStmt->accept(*this);
        
        // 如果Else部分没有终止指令，添加跳转到merge块
        if (!builder->GetInsertBlock()->getTerminator()) {
            builder->CreateBr(mergeBlock);
        }
    }
    
    // 8. 将merge块添加到函数并设置插入点
    mergeBlock->insertInto(function);
    builder->SetInsertPoint(mergeBlock);
}

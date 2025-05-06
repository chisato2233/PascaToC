#pragma once


#include "ASTBase.h"
#include <unordered_map>
#include <stack>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include "Statements/BlockStmt.h"
#include "Expression/Expression.h"
#include "SymbolTable.h"


// 函数声明节点
class FunctionDeclaration : public ASTAcceptImpl<FunctionDeclaration, Declaration> {
public:
    std::string funcName;                       // 函数名
    std::string returnType;                     // 返回类型
    std::vector<ParameterInfo> parameters;      // 参数列表
    std::shared_ptr<Statement> body;            // 函数体

    FunctionDeclaration(const std::string& name, std::string returnType,
                       std::vector<ParameterInfo> params, std::shared_ptr<Statement> body)
        : funcName(name), returnType(std::move(returnType)), 
          parameters(std::move(params)), body(std::move(body)) {
    }

    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Function Declaration: {} -> {}", padding, funcName, returnType);
        
        if (!parameters.empty()) {
            spdlog::info("{}  Parameters:", padding);
            for (const auto& param : parameters) {
                std::string paramType = param.isRef ? "var " + param.type : param.type;
                spdlog::info("{}    - {}: {}", padding, param.name, paramType);
            }
        }
        
        if (body) {
            spdlog::info("{}  Body:", padding);
            body->printAST(indent + 2);
        }
    }
};

_VisitDecl_(CCodeGenVisitor, FunctionDeclaration) {
    // 添加函数到符号表
    CCodeGenVisitor::current_function_name = node.funcName;
    auto funcInfo = std::make_shared<FunctionInfo>(node.funcName, node.returnType);
    for (const auto& param : node.parameters) {
        funcInfo->addParameter(param.name, param.type, param.isRef);
    }    
    // 生成函数声明
    output << node.returnType << " " << node.funcName << "(";
    
    // 生成参数列表
    for (size_t i = 0; i < node.parameters.size(); ++i) {
        const auto& param = node.parameters[i];
        
        // 处理引用参数（在C中使用指针）
        if (param.isRef) {
            output << param.type << "* " << param.name;
        } else {
            output << param.type << " " << param.name;
        }
        
        if (i < node.parameters.size() - 1) {
            output << ", ";
        }
    }
    output << ") ";
    GlobalSymbolTable.addSymbol(funcInfo);
    // 生成函数体
    // 先进入新的作用域
    GlobalSymbolTable.enterScope();
    
    // 添加参数到符号表
    for (const auto& param : node.parameters) {
        GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(
            param.name, param.isRef ? param.type + "*" : param.type, param.isRef));
    }
    
    // 生成函数体代码
    auto function_info = std::static_pointer_cast<FunctionInfo>(
        GlobalSymbolTable.lookupSymbol(node.funcName)
    );
    if (node.body) {
        auto block_stmt = std::static_pointer_cast<BlockStmt>(node.body);
        if(block_stmt){
            output<<" {\n";
            if(node.returnType!="void"){
                output<<node.returnType<<" __fun_return_"<<node.funcName<<";\n";
                function_info->return_expr_identity = "__fun_return_"+node.funcName;
            }

            for(auto& decl : block_stmt->declarations){
                decl->accept(*this);
            }

            block_stmt->body->accept(*this);
            
            if(node.returnType!="void" && !function_info->return_expr_identity.empty()){
                output << "return " << function_info->return_expr_identity << ";";
            }

            output << "}\n";
        }else node.body->accept(*this);
    } else {
        output << "{\n  // Empty function body\n}\n";
    }
    
    // 退出作用域
    GlobalSymbolTable.exitScope();
    CCodeGenVisitor::current_function_name = "";
}

_VisitDecl_(LlvmVisitor, FunctionDeclaration) {
    // 1. 创建函数类型
    std::vector<llvm::Type*> paramTypes;
    for (const auto& param : node.parameters) {
        llvm::Type* paramType = getllvmType(param.type);
        if (param.isRef) {
            paramType = paramType->getPointerTo(); // 引用参数使用指针类型
        }
        paramTypes.push_back(paramType);
    }

    // 获取返回类型
    llvm::Type* returnType = node.returnType == "void" ? 
        llvm::Type::getVoidTy(*context) : 
        getllvmType(node.returnType);

    // 创建函数类型
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        returnType,
        paramTypes,
        false  // 不是可变参数
    );

    // 2. 创建函数
    llvm::Function* function = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        node.funcName,
        module.get()
    );

    // 3. 设置参数名称
    size_t idx = 0;
    for (auto& arg : function->args()) {
        arg.setName(node.parameters[idx].name);
        idx++;
    }

    // 4. 创建基本块
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(
        *context,
        "entry",
        function
    );
    builder->SetInsertPoint(entryBlock);

    // 5. 保存当前函数上下文
    llvm::Function* previousFunction = currentFunction;
    currentFunction = function;

    // 6. 为参数创建局部变量并存储参数值
    idx = 0;
    for (auto& arg : function->args()) {
        const auto& param = node.parameters[idx];
        
        llvm::AllocaInst* alloca = builder->CreateAlloca(
            arg.getType(),
            nullptr,
            param.name + "_local"
        );
        
        // 存储参数值
        builder->CreateStore(&arg, alloca);
        
        // 记录局部变量信息
        setLLVMValueInfo(param.name, alloca, arg.getType(), param.isRef);
        
        idx++;
    }

    // 7. 如果有返回值，创建返回值变量
    llvm::AllocaInst* returnValue = nullptr;
    if (node.returnType != "void") {
        returnValue = builder->CreateAlloca(
            returnType,
            nullptr,
            "return_value"
        );
    }

    // 8. 生成函数体
    if (node.body) {
        // 设置返回值变量
        this->currentReturnValue = returnValue;
        // 访问函数体
        node.body->accept(*this);
    }

    // 9. 确保函数正确返回
    if (!builder->GetInsertBlock()->getTerminator()) {
        if (node.returnType == "void") {
            builder->CreateRetVoid();
        } else {
            // 加载并返回返回值
            llvm::Value* retVal = builder->CreateLoad(
                returnType,
                returnValue,
                "return_value_final"
            );
            builder->CreateRet(retVal);
        }
    }

    // 10. 恢复之前的函数上下文
    currentFunction = previousFunction;

    // 11. 记录函数
    setLLVMFunctionInfo(node.funcName, function);
}









// 过程声明节点
class ProcedureDeclaration : public ASTAcceptImpl<ProcedureDeclaration, Declaration> {
public:
    FunctionDeclaration actual_decl;

    ProcedureDeclaration(const std::string& name, std::vector<ParameterInfo> params, 
                         std::shared_ptr<Statement> body)
            : actual_decl(name, "void",std::move(params), std::move(body)) {
    }

    void printAST(int indent = 0) const override {
        actual_decl.printAST(indent);
    }
};

_VisitDecl_(CCodeGenVisitor, ProcedureDeclaration) {
    node.actual_decl.accept(*this);
}
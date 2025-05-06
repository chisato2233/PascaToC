// **变量引用表达式**
#pragma once
#include "ASTBase.h"
#include "SymbolTable.h"
#include "spdlog/spdlog.h"

class VariableExpr : public ASTAcceptImpl<VariableExpr,Expression> {
public:
    std::string name;
    explicit VariableExpr(const std::string& name) : name(name) {}
    std::string get_name() const noexcept override {return "Variable Expression [" + name +"]";}
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Variable: {}", padding, name);
    }

    ExprType getType() const override {
        auto symbol = GlobalSymbolTable.lookupSymbol(name);
        if (!symbol) return ExprType::Unknown;
        
        if (symbol->type == "long long") return ExprType::Integer;
        if (symbol->type == "double" || symbol->type == "float") return ExprType::Real;
        if (symbol->type == "char") return ExprType::String;
        if (symbol->type == "bool") return ExprType::Boolean;
        // 其他类型处理...
        
        return ExprType::Unknown;
    }
};

_VisitDecl_(CCodeGenVisitor,VariableExpr){
    // 查找符号表
    auto symbol = GlobalSymbolTable.lookupSymbol(node.name);
    
    if (symbol && symbol->symbolType == SymbolType::Function) {
        // 如果是函数，并且不是在赋值语句的左侧
        if (!isLeftSideOfAssignment) {
            output << node.name << "()";
        } else {
            output << node.name;
        }
    } else {
        if(symbol && symbol->symbolType== SymbolType::Variable && std::static_pointer_cast<VariableInfo>(symbol)->isReference){
            output <<"*"<< node.name;
        }else{
            output << node.name;
        }
    }
}

_VisitDecl_(LlvmVisitor, VariableExpr) {
    // 先尝试获取函数信息
    if (auto* funcInfo = getLLVMFunctionInfo(node.name)) {
        setLastValue(builder->CreateCall(funcInfo->function, {}));
        setLastValueType(funcInfo->function->getReturnType());
        return;
    }

    // 如果不是函数，则获取变量信息
    const auto* valueInfo = getLLVMValueInfo(node.name);
    if (!valueInfo) {
        spdlog::error("No LLVM value found for: {}", node.name);
        return;
    }

    // 处理变量
    if (valueInfo->isReference) {
        // 引用类型：先加载指针，再加载值
        llvm::Value* ptrValue = builder->CreateLoad(
            valueInfo->allocatedType->getPointerTo(),
            valueInfo->value,
            node.name + "_ptr"
        );
        setLastValue(builder->CreateLoad(
            valueInfo->allocatedType,
            ptrValue,
            node.name + "_value"
        ));
        setLastValueType(valueInfo->allocatedType);
    } else {
        // 普通变量：直接加载值
        setLastValue(builder->CreateLoad(
            valueInfo->allocatedType,
            valueInfo->value,
            node.name + "_value"
        ));
        setLastValueType(valueInfo->allocatedType);
    }
}

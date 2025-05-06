#pragma once
#include "Expression.h"
#include <llvm/IR/Value.h>

// 数组访问表达式
class ArrayAccessExpr : public ASTAcceptImpl<ArrayAccessExpr, Expression> {
public:
    // 改为使用表达式而不仅仅是字符串名称
    std::shared_ptr<Expression> array;
    std::string arrayName; // 保留用于简单情况
    ExprVec indices;
    bool isNestedAccess = false;

    // 单维数组访问构造函数
    ArrayAccessExpr(const std::string& name, std::shared_ptr<Expression> index)
        : arrayName(name), isNestedAccess(false) {
        array = std::make_shared<VariableExpr>(name);
        indices.push_back(std::move(index));
    }

    // 多维数组访问构造函数
    ArrayAccessExpr(const std::string& name, const ExprVec& indexList)
        : arrayName(name), indices(indexList), isNestedAccess(false) {
        array = std::make_shared<VariableExpr>(name);
    }

    // 嵌套数组访问构造函数
    ArrayAccessExpr(std::shared_ptr<Expression> arrayExpr, std::shared_ptr<Expression> index)
        : array(arrayExpr), isNestedAccess(true) {
        indices.push_back(std::move(index));
    }

    std::string get_name() const noexcept override {
        if (!isNestedAccess) {
            return "Array Access [" + arrayName + "]";
        } else {
            return "Nested Array Access";
        }
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        if (!isNestedAccess) {
            spdlog::info("{}+ Array Access: {}", padding, arrayName);
        } else {
            spdlog::info("{}+ Nested Array Access:", padding);
            array->printAST(indent + 2);
        }
        spdlog::info("{}  Indices:", padding);
        for (const auto& idx : indices) {
            idx->printAST(indent + 2);
        }
    }

    ExprType getType() const override {
        // 嵌套访问的处理更复杂，可能需要根据索引数量递归确定类型
        if (isNestedAccess) {
            // 简化处理，假设结果是整数类型
            return ExprType::Integer;
        }
        
        // 原来的处理逻辑
        auto symbol = GlobalSymbolTable.lookupSymbol(arrayName);
        if (!symbol) return ExprType::Unknown;
        
        if (symbol->type.find("long long") != std::string::npos) return ExprType::Integer;
        if (symbol->type.find("double") != std::string::npos) return ExprType::Real;
        if (symbol->type.find("char") != std::string::npos) return ExprType::String;
        if (symbol->type.find("bool") != std::string::npos) return ExprType::Boolean;
        
        return ExprType::Unknown;
    }
};

_VisitDecl_(CCodeGenVisitor, ArrayAccessExpr) {
    if (node.isNestedAccess) {
        node.array->accept(*this);
    } else {
        output << node.arrayName;
    }
    
    for (const auto& index : node.indices) {
        output << "[";
        index->accept(*this);
        output << "]";
    }
}

_VisitDecl_(LlvmVisitor, ArrayAccessExpr) {
    //----------------------------------------------------------------------
    // 1) 取数组起始指针 & 元素 Pascal 类型
    //----------------------------------------------------------------------
    llvm::Value* arrayPtr = nullptr;
    llvm::Type* elemType = nullptr;

    if (node.isNestedAccess) {               // 形如 a[i][j]
        node.array->accept(*this);
        arrayPtr     = getLastValue();
        elemType  = getLastValueType();
    } else {                                 // 顶层标识符
        const auto* vi = getLLVMValueInfo(node.arrayName);
        if (!vi) {
            spdlog::error("Array not found: {}", node.arrayName);
            return;
        }
        arrayPtr    = vi->value;
        elemType = vi->allocatedType;
    }

    if (!arrayPtr) {
        spdlog::error("Failed to get array pointer");
        return;
    }

    std::vector<llvm::Value*> gepIdx;
    gepIdx.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0));

    for (const auto& idxExpr : node.indices) {
        idxExpr->accept(*this);
        llvm::Value* idx = getLastValue();
        if (!idx) {
            spdlog::error("Failed to evaluate array index");
            return;
        }
        if (!idx->getType()->isIntegerTy())
            idx = builder->CreateFPToSI(idx,
                     llvm::Type::getInt32Ty(*context), "idx_to_int");
        gepIdx.push_back(idx);
    }


    while (llvm::isa<llvm::ArrayType>(elemType) && gepIdx.size() > 1) {
        elemType = llvm::cast<llvm::ArrayType>(elemType)->getElementType();
    }


    llvm::Value* elementPtr =
        builder->CreateInBoundsGEP(elemType, arrayPtr, gepIdx, "arr.elem.ptr");

    //----------------------------------------------------------------------
    // 5) 左值 / 右值
    //----------------------------------------------------------------------
    if (isLeftHandSide) {
        setLastValue(elementPtr);
        setLastValueType(elemType);
    } else {
        llvm::Value* loaded =
            builder->CreateLoad(elemType, elementPtr, "arr.elem");
        setLastValue(loaded);
        setLastValueType(elemType);
    }
}

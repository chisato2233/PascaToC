#pragma once
#include "Expression.h"

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
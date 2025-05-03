#pragma once
#include "Expression.h"

// 数组访问表达式
class ArrayAccessExpr : public ASTAcceptImpl<ArrayAccessExpr, Expression> {
public:
    std::string arrayName;
    ExprVec indices;

    // 单维数组访问构造函数
    ArrayAccessExpr(const std::string& name, std::shared_ptr<Expression> index)
        : arrayName(name) {
        indices.push_back(std::move(index));
    }

    // 多维数组访问构造函数
    ArrayAccessExpr(const std::string& name, const ExprVec& indexList)
        : arrayName(name), indices(indexList) {
    }

    std::string get_name() const noexcept override {
        return "Array Access [" + arrayName + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Array Access: {}", padding, arrayName);
        spdlog::info("{}  Indices:", padding);
        for (const auto& idx : indices) {
            idx->printAST(indent + 2);
        }
    }

    ExprType getType() const override {
        // 通过符号表查找数组类型
        auto symbol = GlobalSymbolTable.lookupSymbol(arrayName);
        if (!symbol) return ExprType::Unknown;
        
        // 根据数组基础类型返回
        if (symbol->type.find("int") != std::string::npos) return ExprType::Integer;
        if (symbol->type.find("double") != std::string::npos) return ExprType::Real;
        if (symbol->type.find("char") != std::string::npos) return ExprType::String;
        if (symbol->type.find("bool") != std::string::npos) return ExprType::Boolean;
        
        return ExprType::Unknown;
    }
};

_VisitDecl_(CCodeGenVisitor, ArrayAccessExpr) {
    output << node.arrayName;
    for (const auto& index : node.indices) {
        output << "[";
        index->accept(*this);
        output << "]";
    }
} 
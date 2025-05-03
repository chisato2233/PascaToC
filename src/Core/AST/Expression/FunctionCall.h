#pragma once
#include "AST/ASTBase.h"
#include "spdlog/spdlog.h"
#include "SymbolTable.h"


#include "Expression/ArrayAccessExpr.h"
#include "Expression/VariableExpr.h"
class FunctionCall : public ASTAcceptImpl<FunctionCall,Expression> {
public:
    std::string funcName;
    std::vector<std::shared_ptr<Expression>> args;
    FunctionCall(const std::string& name, std::vector<std::shared_ptr<Expression>> args)
        : funcName(name), args(std::move(args)) {
    }

    std::string get_name() const noexcept override {
        return "Function Call [" + funcName + "]";
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Function Call: {}", padding, funcName);
        if (!args.empty()) {
            spdlog::info("{}  Arguments:", padding);
            for (const auto& arg : args) {
                arg->printAST(indent + 2);
            }
        }
    }

    ExprType getType() const override {
        auto symbol = GlobalSymbolTable.lookupSymbol(funcName);
        if (!symbol || symbol->symbolType != SymbolType::Function) {
            return ExprType::Unknown;
        }
        
        auto funcInfo = std::dynamic_pointer_cast<FunctionInfo>(symbol);
        if (!funcInfo) return ExprType::Unknown;
        
        if (funcInfo->type == "long long") return ExprType::Integer;
        if (funcInfo->type == "double") return ExprType::Real;
        if (funcInfo->type == "int") return ExprType::Boolean;
        if (funcInfo->type == "char") return ExprType::String;
        return ExprType::Unknown;
    }
};

_VisitDecl_(CCodeGenVisitor, FunctionCall) {
    output << node.funcName << "(";
    
    // 获取函数信息，了解哪些参数是引用参数
    auto funcInfo = GlobalSymbolTable.lookupSymbol(node.funcName);
    if(!funcInfo){
        spdlog::error("函数{}不存在", node.funcName);
        return;
    }
    std::vector<ParameterInfo>* paramInfos = nullptr;
    
    if (funcInfo && (funcInfo->symbolType == SymbolType::Function || 
                    funcInfo->symbolType == SymbolType::Procedure))
    {
        auto funcSymbol = std::static_pointer_cast<FunctionInfo>(funcInfo);
        paramInfos = &funcSymbol->parameters;
    }
    
    if(!paramInfos || paramInfos->size() != node.args.size()){
        spdlog::error("函数{}的参数数量与实际参数数量不匹配", node.funcName);
        return;
    }
    // 处理参数
    for (size_t i = 0; i < node.args.size(); ++i) {
        if (i > 0) output << ", ";
        
        auto& arg = node.args[i];
        bool isParamRef = (i < paramInfos->size() && paramInfos->at(i).isRef);
        
        // 检查数组访问先处理
        if (auto arrayAccessExpr = std::dynamic_pointer_cast<ArrayAccessExpr>(arg)) {

            if (isParamRef) {
                // 引用参数接收数组元素 - 取地址
                output << "&";
                arrayAccessExpr->accept(*this);
            } else {
                // 非引用参数接收数组元素 - 正常访问
                arrayAccessExpr->accept(*this);
            }
            continue; // 跳过后续处理
        }
        
        // 检查变量引用类型
        if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(arg)) {
            std::string argName = varExpr->name;
            auto argInfo = GlobalSymbolTable.lookupSymbol(argName);
            bool isArgRef = false;
            
            if (argInfo && argInfo->symbolType == SymbolType::Variable) {
                auto varInfo = std::static_pointer_cast<VariableInfo>(argInfo);
                isArgRef = varInfo->isReference;
            }
            
            if (isParamRef) {
                // 引用参数
                if (isArgRef) {
                    // 引用参数接收引用变量 - 应该去掉默认解引用，直接传递原变量名
                    output << argName; // 不使用accept()，避免自动添加*
                } else {
                    // 引用参数接收普通变量 - 取地址
                    output << "&" << argName;
                }
            } else {
                // 非引用参数
                if (isArgRef) {
                    // 非引用参数接收引用变量 - VariableExpr已经添加*，直接使用
                    varExpr->accept(*this); // 默认会生成*argName
                } else {
                    // 普通参数接收普通变量
                    varExpr->accept(*this);
                }
            }
        } else {
            if (isParamRef) output << "&(";
            arg->accept(*this);
            if (isParamRef) output << ")";
        }
        
    }
    
    output << ")";
}


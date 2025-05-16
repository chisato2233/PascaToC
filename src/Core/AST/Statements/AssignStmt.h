#pragma once
#include "ASTBase.h"
#include "SymbolTable.h"
#include "Expression/RecordFieldExpr.h"
// **赋值语句**
class AssignmentStmt : public ASTAcceptImpl<AssignmentStmt,Statement> {
public:
    std::string variable;
    std::shared_ptr<Expression> expr;
    AssignmentStmt(const std::string& var, std::shared_ptr<Expression> expr)
        : variable(var), expr(std::move(expr)) {
        }
    std::string get_name() const noexcept override {return "Assignment Statement";}
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Assignment: {} =", padding, variable);
        expr->printAST(indent + 1);
    }
};

_VisitDecl_(CCodeGenVisitor,AssignmentStmt){
    // 设置上下文信息，表示下一个表达式是在赋值语句左侧

    if(node.variable == CCodeGenVisitor::current_function_name){
        auto function_info = std::static_pointer_cast<FunctionInfo>(
            GlobalSymbolTable.lookupSymbol(node.variable)
        );

        if(function_info && function_info->returnType!="void"){
            if(function_info->return_expr_identity.empty()){
                auto expr_type = node.expr->getCType();
                auto identity = node.variable + "_return_expr__";
                function_info->return_expr_identity = identity;
                output << node.expr->getCType() << " ";
            }
            output<<function_info->return_expr_identity << " = ";
            node.expr->accept(*this);
            output << ";";
        }else{
            output << "return;";
        }
        
    }else{
        bool oldValue = isLeftSideOfAssignment;
        isLeftSideOfAssignment = true;
            
        // 检查变量是否为引用
        bool isRef = false;
        auto symbolInfo = GlobalSymbolTable.lookupSymbol(node.variable);
        if (symbolInfo && symbolInfo->symbolType == SymbolType::Variable) {
            auto varInfo = std::static_pointer_cast<VariableInfo>(symbolInfo);
            isRef = varInfo->isReference;
        }
        
        // 如果是引用变量，在C中它实际上是指针类型
        if (isRef) {
            output << "*" << node.variable << " = ";  // 在赋值左侧，需要解引用
        } else {
            output << node.variable << " = ";
        }
        
        // 恢复上下文信息，右侧不再是赋值语句左侧
        isLeftSideOfAssignment = false;
        
        // 生成右侧表达式
        node.expr->accept(*this);
        output << ";";
        
        // 恢复原始上下文
        isLeftSideOfAssignment = oldValue;
    }
}

_VisitDecl_(LlvmVisitor, AssignmentStmt) {
    // 检查是否是为函数的返回值赋值
    if (node.variable == currentFunction->getName().str()) {
        // 当前函数的返回值赋值
        llvm::AllocaInst* returnValue = currentReturnValue;
        if (!returnValue) {
            spdlog::error("No return value allocated for function {}", node.variable);
            return;
        }

        // 计算右侧表达式的值
        node.expr->accept(*this);
        llvm::Value* exprValue = getLastValue();
        if (!exprValue) {
            spdlog::error("Failed to evaluate return expression");
            return;
        }

        // 存储值到返回变量
        builder->CreateStore(exprValue, returnValue);
    } else {
        // 普通变量赋值
        // 设置上下文标志，表示是赋值语句左侧
        bool oldValue = isLeftHandSide;
        isLeftHandSide = true;

        // 获取左侧变量信息
        const auto* valueInfo = getLLVMValueInfo(node.variable);
    
        // 普通变量处理
        llvm::Value* targetPtr = valueInfo->value;
        llvm::Type* targetType = valueInfo->allocatedType;
        // 处理引用类型 - 透明指针设计下的调整
        if (valueInfo->isReference) {
            // 如果是引用，我们需要先加载指针值
            targetPtr = builder->CreateLoad(targetType->getPointerTo(), targetPtr, node.variable + "_ref");
        }
        
        // 恢复上下文标志
        isLeftHandSide = false;
        
        // 计算右侧表达式的值
        node.expr->accept(*this);
        llvm::Value* exprValue = getLastValue();
        if (!exprValue) {
            spdlog::error("Failed to evaluate expression in assignment");
            isLeftHandSide = oldValue;
            return;
        }
        
            // 存储值到目标
        builder->CreateStore(exprValue, targetPtr);
        
        
        // 恢复原始上下文
        isLeftHandSide = oldValue;
    }
}


// 记录字段赋值语句
class FieldAssignmentStmt : public ASTAcceptImpl<FieldAssignmentStmt, Statement> {
public:

    std::shared_ptr<RecordFieldExpr> fieldExpr;
    std::shared_ptr<Expression> value;

    FieldAssignmentStmt(std::shared_ptr<RecordFieldExpr> field, std::shared_ptr<Expression> expr)
        : fieldExpr(std::move(field)), value(std::move(expr)) {}
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Field Assignment:", padding);
        fieldExpr->printAST(indent + 2);
        spdlog::info("{}  Value:", padding);
        value->printAST(indent + 2);
    }
};

_VisitDecl_(CCodeGenVisitor, FieldAssignmentStmt) {
    node.fieldExpr->accept(*this);
    output << " = ";
    node.value->accept(*this);
    output << ";\n";
}

// _VisitDecl_(LlvmVisitor, FieldAssignmentStmt) {
//     // 获取记录变量
//     llvm::Value* recordPtr = getLLVMValue(node.fieldExpr->recordName);
//     if (!recordPtr) {
//         spdlog::error("Record variable not found: {}", node.fieldExpr->getRecordName());
//         return;
//     }
    
//     // 获取记录类型信息
//     std::string recordTypeName = getLLVMValueType(node.fieldExpr->getRecordName());
    
//     // 获取字段索引
//     int fieldIndex = getLLVMStructFieldIndex(recordTypeName, node.fieldExpr->getFieldName());
//     if (fieldIndex < 0) {
//         spdlog::error("Field not found: {} in record {}", 
//                      node.fieldExpr->getFieldName(), node.fieldExpr->getRecordName());
//         return;
//     }
    
//     // 创建GEP指令获取字段指针
//     llvm::Value* fieldPtr = builder->CreateStructGEP(
//         getLLVMStructType(recordTypeName),
//         recordPtr,
//         fieldIndex,
//         node.fieldExpr->getRecordName() + "." + node.fieldExpr->getFieldName()
//     );
    
//     // 计算右侧表达式的值
//     llvm::Value* valueToStore = node.value->accept(*this);
//     if (!valueToStore) {
//         spdlog::error("Failed to evaluate field assignment value");
//         return;
//     }
    
//     // 存储值到字段
//     builder->CreateStore(valueToStore, fieldPtr);
// }



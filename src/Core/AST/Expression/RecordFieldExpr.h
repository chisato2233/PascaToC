#include "ASTBase.h"
#include "SymbolTable.h"
#include "AST/Expression/VariableExpr.h"

// 记录字段访问表达式
class RecordFieldExpr : public ASTAcceptImpl<RecordFieldExpr, Expression> {
public:

    std::shared_ptr<Expression> record;  // 可以是变量或另一个字段访问
    std::string fieldName;               // 字段名

    // 构造函数：从变量名创建
    RecordFieldExpr(const char* recordName, const char* field)
        : record(std::make_shared<VariableExpr>(recordName)), fieldName(field) {}
    
    // 构造函数：从表达式创建（支持嵌套）
    RecordFieldExpr(std::shared_ptr<Expression> recordExpr, const std::string& field)
        : record(std::move(recordExpr)), fieldName(field) {}
        
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Record Field Access: {}", padding, fieldName);
        record->printAST(indent + 2);
    }
    
    virtual std::string getCType() const override {
        // 递归获取记录表达式的类型
        std::string recordType;
        
        if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(record)) {
            // 直接变量访问
            auto varInfo = std::dynamic_pointer_cast<VariableInfo>(
                GlobalSymbolTable.lookupSymbol(varExpr->name));
            if (!varInfo) {
                spdlog::error("Variable not found: {}", varExpr->name);
                return "error";
            }
            recordType = varInfo->type; 
        } else if (auto fieldExpr = std::dynamic_pointer_cast<RecordFieldExpr>(record)) {
            // 嵌套字段访问
            recordType = fieldExpr->getCType();
        } else {
            spdlog::error("Unsupported record expression type");
            return "error";
        }
        
        // 查找记录类型定义
        auto typeInfo = std::dynamic_pointer_cast<RecordTypeInfo>(
            GlobalSymbolTable.lookupSymbol(recordType));
        if (!typeInfo) {
            spdlog::error("Record type not found: {}", recordType);
            return "error";
        }
        
        // 查找字段定义
        const RecordFieldInfo* field = typeInfo->findField(fieldName);
        if (!field) {
            spdlog::error("Field not found: {} in record type {}", fieldName, recordType);
            return "error";
        }
        
        return field->type;
    }
};


_VisitDecl_(CCodeGenVisitor, RecordFieldExpr) {
    node.record->accept(*this);
    output << "." << node.fieldName;
}
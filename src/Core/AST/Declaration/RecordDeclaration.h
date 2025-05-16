#include "AST/ASTBase.h"
#include "SymbolTable.h"


// 记录类型定义节点
class RecordTypeDeclaration : public ASTAcceptImpl<RecordTypeDeclaration, Declaration> {
public:
    std::string typeName;                     // 类型名
    std::vector<RecordFieldInfo> fields;      // 字段列表
    
    RecordTypeDeclaration(std::string name, std::vector<RecordFieldInfo> fieldList)
        : typeName(std::move(name)), fields(std::move(fieldList)) {}
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Record Type Declaration: {}", padding, typeName);
        
        if (!fields.empty()) {
            spdlog::info("{}  Fields:", padding);
            for (const auto& field : fields) {
                spdlog::info("{}    - {}: {}", padding, field.name, field.type);
            }
        }
    }
};


_VisitDecl_(CCodeGenVisitor, RecordTypeDeclaration) {
    // 生成C语言结构体定义
    output << "typedef struct {\n";
    
    // 生成字段列表
    for (const auto& field : node.fields) {
        output << "    " << field.type << " " << field.name << ";\n";
    }
    
    output << "} " << node.typeName << ";\n";
    
    // 添加类型信息到符号表
    auto recordTypeInfo = std::make_shared<RecordTypeInfo>(node.typeName, node.fields);
    GlobalSymbolTable.addSymbol(recordTypeInfo);
}


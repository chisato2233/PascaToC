#pragma once
#include "ASTBase.h"
#include<unordered_map>

using VariantTable = std::unordered_map<std::string,std::string>;
class VarDeclaration : public Declaration,_CRTPHelp_(VarDeclaration) {
public:
    _GenerateAccept_()
    std::vector<std::string> varNames; // 变量名称
    std::string typeName;              // 变量类型
    inline static VariantTable GlobalTable;

    VarDeclaration(std::vector<std::string> names, std::string type)
        : varNames(std::move(names)), typeName(std::move(type)) {
        log("VarDeclaration Construct");
    }

    std::string codeGen() override {
        std::ostringstream out;
        for (const auto& name : varNames) {
            GlobalTable[name] = typeName;
            out << typeName << " " << name << ";\n";
        }
        return out.str();
    }
};

_VisitDecl_(CCodeGenVisitor,VarDeclaration){
    for (const auto& name : node->varNames) {
        VarDeclaration::GlobalTable[name] = node->typeName;
        output << node->typeName << " " << name << ";\n";
    }
}



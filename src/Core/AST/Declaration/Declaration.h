#pragma once
#include "ASTBase.h"

class VarDeclaration : public Declaration {
private:
    std::vector<std::string> varNames; // 变量名称
    std::string typeName;              // 变量类型

public:
    VarDeclaration(std::vector<std::string> names, std::string type)
        : varNames(std::move(names)), typeName(std::move(type)) {
        log("VarDeclaration Construct");
    }

    std::string codeGen() override {
        std::ostringstream out;
        for (const auto& name : varNames) {
            out << typeName << " " << name << ";\n";
        }
        return out.str();
    }
};
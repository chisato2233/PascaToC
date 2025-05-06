#include "ASTBase.h"
#include "LlvmVisitor.h"
#include "SymbolTable.h"


// 变量声明节点（支持普通变量和数组变量）
// 如果 typeName 含有 '[' 则认为是数组类型，例如 "int[10]"
class VarDeclaration : public ASTAcceptImpl<VarDeclaration,Declaration> {
public:
    std::vector<std::string> varNames;
    std::string typeName; // 可能为 "int" 或形如 "int[10]"
    bool isRef = false;
    VarDeclaration(std::vector<std::string> names, std::string type, bool ref = false)
        : varNames(std::move(names)), typeName(std::move(type)), isRef(ref) {
    }
    
    void printAST(int indent = 0) const override {
        std::string padding = getIndent(indent);
        spdlog::info("{}+ Variable Declaration: {}", padding, typeName);
        for (const auto& name : varNames) {
            spdlog::info("{}  - {}", padding, name);
        }
    }
};

_VisitDecl_(CCodeGenVisitor, VarDeclaration) {
    size_t pos = node.typeName.find('[');
    if (pos != std::string::npos) {
        std::string baseType = node.typeName.substr(0, pos);
        std::string arraySuffix = node.typeName.substr(pos);
        for (const auto &name : node.varNames) {
            GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(name, node.typeName));
            output << baseType << " " << name << arraySuffix << ";\n";
        }
    } else {
        for (const auto &name : node.varNames) {
            GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(name, node.typeName, node.isRef));
            output << node.typeName << " " << name << ";\n";
        }
    }
}

_VisitDecl_(LlvmVisitor, VarDeclaration) {
    // 检查是否是数组类型
    size_t pos = node.typeName.find('[');
    if (pos != std::string::npos) {
        // 数组类型处理
        std::string baseType = node.typeName.substr(0, pos);
        std::string arraySizeStr = node.typeName.substr(pos + 1);
        arraySizeStr = arraySizeStr.substr(0, arraySizeStr.find(']'));
        int arraySize = std::stoi(arraySizeStr);

        // 获取基础类型的LLVM Type
        llvm::Type* elementType = getllvmType(baseType);
        if (!elementType) {
            spdlog::error("Unsupported array element type: {}", baseType);
            return;
        }

        // 创建数组类型
        llvm::ArrayType* arrayType = llvm::ArrayType::get(elementType, arraySize);
        
        for (const auto& name : node.varNames) {
            llvm::Value* arrayAlloca = builder->CreateAlloca(arrayType, nullptr, name);
            setLLVMValueInfo(name, arrayAlloca, arrayType, false);    
        }
    } else {
        // 普通变量处理
        llvm::Type* baseType = getllvmType(node.typeName);
        if (!baseType) {
            spdlog::error("Unsupported variable type: {}", node.typeName);
            return;
        }

        for (const auto& name : node.varNames) {
            llvm::Type* allocType = node.isRef ? 
                baseType->getPointerTo() :  // 如果是引用，创建指针类型
                baseType;                   // 否则创建普通类型

            llvm::Value* varAlloca = builder->CreateAlloca(allocType, nullptr, name);
            setLLVMValueInfo(name, varAlloca, allocType, node.isRef);
            
        }
    }
}
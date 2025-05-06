#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <memory>

#include "VisitorBase.h"
#include "Core/AST/SymbolTable.h"

class LlvmVisitor : public VisitImplCRTP<LlvmVisitor> {
private:
    // LLVM核心组件
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    
    // 用于存储上一次访问的返回值
    llvm::Value* lastValue;
    
    // LLVM值符号表：将符号名映射到对应的LLVM Value
    std::unordered_map<std::string, llvm::Value*> llvmValues;
    
    // LLVM函数符号表：将函数名映射到对应的LLVM Function
    std::unordered_map<std::string, llvm::Function*> llvmFunctions;

    // Pascal类型到LLVM类型的映射
    llvm::Type* getllvmType(const std::string& cType) {
        if (cType == "long long") {
            return llvm::Type::getInt64Ty(*context);
        } else if (cType == "double") {
            return llvm::Type::getDoubleTy(*context);
        } else if (cType == "int") {
            return llvm::Type::getInt32Ty(*context);
        } else if (cType == "char") {
            return llvm::Type::getInt8Ty(*context);
        }
        // 添加更多类型映射...
        return nullptr;
    }

public:
    LlvmVisitor(const std::string& moduleName = "pascal_module") 
        : lastValue(nullptr) {
        // 初始化LLVM核心组件
        context = std::make_unique<llvm::LLVMContext>();
        module = std::make_unique<llvm::Module>(moduleName, *context);
        builder = std::make_unique<llvm::IRBuilder<>>(*context);
    }

    // 获取上一次访问的返回值
    llvm::Value* getLastValue() const { return lastValue; }
    void setLastValue(llvm::Value* value) { lastValue = value; }

    // 获取LLVM核心组件
    llvm::LLVMContext& getContext() { return *context; }
    llvm::Module& getModule() { return *module; }
    llvm::IRBuilder<>& getBuilder() { return *builder; }

    // LLVM值管理
    void setLLVMValue(const std::string& name, llvm::Value* value) {
        llvmValues[name] = value;
    }

    llvm::Value* getLLVMValue(const std::string& name) {
        auto it = llvmValues.find(name);
        if (it != llvmValues.end()) {
            return it->second;
        }
        return nullptr;
    }

    // LLVM函数管理
    void setLLVMFunction(const std::string& name, llvm::Function* func) {
        llvmFunctions[name] = func;
    }

    llvm::Function* getLLVMFunction(const std::string& name) {
        auto it = llvmFunctions.find(name);
        if (it != llvmFunctions.end()) {
            return it->second;
        }
        return nullptr;
    }

    // 创建LLVM变量
    llvm::Value* createVariable(const std::string& name, const std::string& type, bool isGlobal = false) {
        llvm::Type* llvmType = getllvmType(type);
        if (!llvmType) return nullptr;

        if (isGlobal) {
            // 创建全局变量
            auto global = new llvm::GlobalVariable(
                *module,
                llvmType,
                false, // 是否是常量
                llvm::GlobalValue::ExternalLinkage,
                llvm::Constant::getNullValue(llvmType),
                name
            );
            setLLVMValue(name, global);
            return global;
        } else {
            // 创建局部变量
            llvm::Value* alloca = builder->CreateAlloca(llvmType, nullptr, name);
            setLLVMValue(name, alloca);
            return alloca;
        }
    }

    // 获取生成的LLVM IR
    std::string getIR() {
        std::string ir;
        llvm::raw_string_ostream os(ir);
        module->print(os, nullptr);
        return ir;
    }
};
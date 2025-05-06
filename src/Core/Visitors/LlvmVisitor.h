#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <memory>

#include "VisitorBase.h"

class LlvmVisitor : public VisitImplCRTP<LlvmVisitor> {
private:
    // LLVM核心组件
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    
    // 用于存储上一次访问的返回值
    llvm::Value* lastValue;
    
    // 记录LLVM值的类型信息
    struct LLVMValueInfo {
        llvm::Value* value;          // LLVM值
        llvm::Type* allocatedType;   // 分配的实际类型
        bool isReference;            // 是否是引用类型
        

        LLVMValueInfo(llvm::Value* v = nullptr, llvm::Type* t = nullptr, bool ref = false) 
            : value(v), allocatedType(t), isReference(ref) {}
    };

    struct LLVMFunctionInfo {
        llvm::Function* function;


        LLVMFunctionInfo(llvm::Function* f = nullptr)
            : function(f) {}
    };
    
    std::unordered_map<std::string, LLVMValueInfo> llvmValueInfos;
    std::unordered_map<std::string, LLVMFunctionInfo> llvmFunctionInfos;


public:
    llvm::Function* currentFunction = nullptr;  // 当前正在处理的函数
    llvm::AllocaInst* currentReturnValue = nullptr;  // 当前函数的返回值变量

    // C类型到LLVM类型的映射
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
            setLLVMValueInfo(name, global, llvmType, false);
            return global;
        } else {
            // 创建局部变量
            llvm::Value* alloca = builder->CreateAlloca(llvmType, nullptr, name);
            setLLVMValueInfo(name, alloca, llvmType, false);
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

    void setLLVMValueInfo(const std::string& name, llvm::Value* value, 
                         llvm::Type* allocType, bool isRef) {
        llvmValueInfos[name] = LLVMValueInfo(value, allocType, isRef);
    }

    const LLVMValueInfo* getLLVMValueInfo(const std::string& name) {
        auto it = llvmValueInfos.find(name);
        return it != llvmValueInfos.end() ? &it->second : nullptr;
    }

    void setLLVMFunctionInfo(const std::string& name, llvm::Function* function) {
        llvmFunctionInfos[name] = LLVMFunctionInfo(function);
    }

    const LLVMFunctionInfo* getLLVMFunctionInfo(const std::string& name) {
        auto it = llvmFunctionInfos.find(name);
        return it != llvmFunctionInfos.end() ? &it->second : nullptr;
    }

    template<typename NodeType>
    void visit(NodeType& node) {
        spdlog::error("No LlvmVisitor method found for node type: {}", typeid(node).name());
    }
};
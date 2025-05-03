#pragma once
#include <string>
#include <unordered_map>
#include <stack>
#include <memory>
#include <vector>
#include "AST/ASTBase.h"
// 符号类型
enum class SymbolType {
    Variable,
    Function,
    Procedure,
    Type,
    Constant
};

// 符号信息基类
class SymbolInfo {
public:
    SymbolType symbolType;
    std::string name;
    std::string type;
    
    SymbolInfo(SymbolType sType, std::string sName, std::string strType)
        : symbolType(sType), name(std::move(sName)), type(std::move(strType)) {}
    
    virtual ~SymbolInfo() = default;
};

// 符号表类
class SymbolTable {
private:
    std::stack<std::unordered_map<std::string, std::shared_ptr<SymbolInfo>>> _scopes;
    
public:
    SymbolTable() {
        // 初始化全局作用域
        enterScope();
    }
    
    // 进入新作用域
    void enterScope() {
        _scopes.push({});
    }
    
    // 退出当前作用域
    void exitScope() {
        if (_scopes.size() > 1) { // 保留全局作用域
            _scopes.pop();
        }
    }
    
    // 添加符号
    bool addSymbol(std::shared_ptr<SymbolInfo> info) {
        auto& currentScope = _scopes.top();
        if (currentScope.find(info->name) != currentScope.end()) {
            return false; // 符号已存在
        }
        currentScope[info->name] = info;
        return true;
    }
    
    // 查找符号（会搜索所有可见作用域）
    std::shared_ptr<SymbolInfo> lookupSymbol(const std::string& name) {
        std::stack<std::unordered_map<std::string, std::shared_ptr<SymbolInfo>>> tempStack = _scopes;
        
        while (!tempStack.empty()) {
            auto& scope = tempStack.top();
            auto it = scope.find(name);
            if (it != scope.end()) {
                return it->second;
            }
            tempStack.pop();
        }
        
        return nullptr; // 未找到符号
    }
    
    // 仅在当前作用域查找符号
    std::shared_ptr<SymbolInfo> lookupSymbolInCurrentScope(const std::string& name) {
        auto& currentScope = _scopes.top();
        auto it = currentScope.find(name);
        if (it != currentScope.end()) {
            return it->second;
        }
        return nullptr;
    }
};

// 全局符号表实例
inline static SymbolTable GlobalSymbolTable;

// 变量符号信息
class VariableInfo : public SymbolInfo {
public:
    bool isReference = false; // 新增：标记变量是否为引用

    VariableInfo(std::string name, std::string type, bool isRef = false)
        : SymbolInfo(SymbolType::Variable, std::move(name), std::move(type)), 
          isReference(isRef) {}
};


// 修改：参数定义，增加isRef字段表示参数是否为引用
class ParameterInfo : public SymbolInfo {
public:
    bool isRef;
    
    ParameterInfo(std::string n, std::string t, bool ref = false) 
        : SymbolInfo(SymbolType::Variable, std::move(n), std::move(t)),
          isRef(ref) {}
};
    
// 函数符号信息
class FunctionInfo : public SymbolInfo {
public:

    std::vector<ParameterInfo> parameters; // 修改为新的参数结构
    
    FunctionInfo(std::string name, std::string returnType)
        : SymbolInfo(SymbolType::Function, std::move(name), std::move(returnType)) {}
    
    // 添加参数的便捷方法
    void addParameter(const std::string& name, const std::string& type, bool isRef = false) {
        parameters.emplace_back(name, type, isRef);
    }
};

// 常量符号信息
class ConstantInfo : public SymbolInfo {
public:
    ExprPtr constExpr; // 存储原始表达式
    
    ConstantInfo(std::string name, std::string type, ExprPtr expr)
        : SymbolInfo(SymbolType::Constant, std::move(name), std::move(type)),
          constExpr(std::move(expr)) {}
};
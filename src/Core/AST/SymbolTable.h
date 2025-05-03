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
    VariableInfo(std::string name, std::string type)
        : SymbolInfo(SymbolType::Variable, std::move(name), std::move(type)) {}
};

// 函数符号信息
class FunctionInfo : public SymbolInfo {
public:
    std::vector<std::pair<std::string, std::string>> parameters; // 参数名和类型
    
    FunctionInfo(std::string name, std::string returnType)
        : SymbolInfo(SymbolType::Function, std::move(name), std::move(returnType)) {}
};

// 常量符号信息
class ConstantInfo : public SymbolInfo {
public:
    ExprPtr constExpr; // 存储原始表达式
    
    ConstantInfo(std::string name, std::string type, ExprPtr expr)
        : SymbolInfo(SymbolType::Constant, std::move(name), std::move(type)),
          constExpr(std::move(expr)) {}
};
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





// ----------（保持您原有 SymbolInfo / VariableInfo 等定义不变）----------

class SymbolTable {
    using SymbolPtr = std::shared_ptr<SymbolInfo>;

    using Scope = std::unordered_map<
        std::string, SymbolPtr,
        std::hash<std::string_view>,
        std::equal_to<>>;

    std::vector<Scope> _scopes;

public:
    SymbolTable() { enterScope(); }


    void enterScope() { _scopes.emplace_back(); }

    void exitScope() {
        if (_scopes.size() > 1) _scopes.pop_back();
    }


    template <typename SymPtr>
    bool addSymbol(SymPtr&& info) {
        auto& cur = _scopes.back();
        auto [it, inserted] =
            cur.emplace(info->name, std::forward<SymPtr>(info));
        return inserted;
    }


    SymbolPtr lookupSymbol(std::string_view name) const noexcept {
        for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
            if (auto found = it->find(std::string(name)); found != it->end())
                return found->second;
        }
        return nullptr;
    }

    SymbolPtr lookupSymbolInCurrentScope(std::string_view name) const noexcept {
        const auto& cur = _scopes.back();
        if (auto it = cur.find(std::string(name)); it != cur.end())
            return it->second;
        return nullptr;
    }
};


class ScopeGuard {
    SymbolTable& tbl;
public:
    explicit ScopeGuard(SymbolTable& t) : tbl(t) { tbl.enterScope(); }
    ~ScopeGuard() { tbl.exitScope(); }
};

// 全局符号表实例
inline static SymbolTable GlobalSymbolTable;

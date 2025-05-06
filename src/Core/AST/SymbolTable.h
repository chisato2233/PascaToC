#pragma once
#include <string>
#include <unordered_map>
#include <stack>
#include <memory>
#include <vector>
#include<memory_resource>

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
        : symbolType(sType), name(sName), type(strType) {}
    
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
    std::string returnType;
    std::string return_expr_identity;
    FunctionInfo(std::string _name, std::string _returnType)
        : SymbolInfo(SymbolType::Function, _name, _returnType),
          returnType(_returnType) {}
    
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




struct TransparentHash  {
    using is_transparent = void;
    size_t operator()(std::string_view sv)      const noexcept { return std::hash<std::string_view>{}(sv); }
    size_t operator()(const std::string&  s)    const noexcept { return std::hash<std::string>{}(s); }
};
struct TransparentEq {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
    bool operator()(const  std::string& a, std::string_view b) const noexcept { return a == b; }
};


class SymbolTable {
    using SymbolPtr = std::shared_ptr<SymbolInfo>;

    struct PmrScope {
        inline static std::pmr::monotonic_buffer_resource arena{4096};
        std::pmr::unordered_map<std::string, SymbolPtr,
                            TransparentHash,
                            TransparentEq> table{ &arena };
        PmrScope() = default;
    };

    std::vector<PmrScope> _scopes;

public:

    SymbolTable() { 
        _scopes.reserve(10);
        enterScope(); 
    }

    void enterScope() { _scopes.emplace_back(); }
    void exitScope()  { if (_scopes.size() > 1) _scopes.pop_back(); }


    template <typename SymPtr>
    bool addSymbol(SymPtr&& info) {
        auto& cur = _scopes.back();
        auto [it, inserted] =
            cur.table.emplace(info->name, std::forward<SymPtr>(info));
        return inserted;
    }


    SymbolPtr lookupSymbol(std::string_view name) const noexcept {
        for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
            if (auto found = it->table.find(std::string(name)); found != it->table.end())
                return found->second;
        }
        return nullptr;
    }

    SymbolPtr lookupSymbolInCurrentScope(std::string_view name) const noexcept {
        const auto& cur = _scopes.back();
        if (auto it = cur.table.find(std::string(name)); it != cur.table.end())
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

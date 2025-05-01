#pragma once
#include "ASTBase.h"
#include <unordered_map>
#include <stack>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

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
inline SymbolTable GlobalSymbolTable;

// 变量声明节点（支持普通变量和数组变量）
// 如果 typeName 含有 '[' 则认为是数组类型，例如 "int[10]"
class VarDeclaration : public Declaration, _CRTPHelp_(VarDeclaration) {
public:
    _GenerateAccept_()
    std::vector<std::string> varNames;
    std::string typeName; // 可能为 "int" 或形如 "int[10]"
    
    VarDeclaration(std::vector<std::string> names, std::string type)
        : varNames(std::move(names)), typeName(std::move(type)) {
        log("VarDeclaration Construct");
    }
    
    std::string codeGen() override {
        std::ostringstream out;
        // 如果 typeName 中含有 '[' 则认为是数组，需将数组维度放在变量名后面
        size_t pos = typeName.find('[');
        if (pos != std::string::npos) {
            std::string baseType = typeName.substr(0, pos);
            std::string arraySuffix = typeName.substr(pos); // 包含 [ 和 ]
            for (const auto& name : varNames) {
                GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(name, typeName));
                out << baseType << " " << name << arraySuffix << ";\n";
            }
        } else {
            // 普通变量声明
            for (const auto& name : varNames) {
                GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(name, typeName));
                out << typeName << " " << name << ";\n";
            }
        }
        return out.str();
    }
};

_VisitDecl_(CCodeGenVisitor, VarDeclaration) {
    size_t pos = node->typeName.find('[');
    if (pos != std::string::npos) {
        std::string baseType = node->typeName.substr(0, pos);
        std::string arraySuffix = node->typeName.substr(pos);
        for (const auto &name : node->varNames) {
            GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(name, node->typeName));
            output << baseType << " " << name << arraySuffix << ";\n";
        }
    } else {
        for (const auto &name : node->varNames) {
            GlobalSymbolTable.addSymbol(std::make_shared<VariableInfo>(name, node->typeName));
            output << node->typeName << " " << name << ";\n";
        }
    }
}

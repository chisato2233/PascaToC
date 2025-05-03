#ifndef PASCAL_PARSER_H
#define PASCAL_PARSER_H

#include <string>
#include <sstream>
#include <memory>
#include "AST.h"
#include <vector>

// 前向声明TokenInfo结构体
struct TokenInfo;

extern std::shared_ptr<ASTNode> getAST();
class PascalParser {
public:
    // 构造函数
    PascalParser();
    
    // 析构函数
    ~PascalParser();
    
    // 解析字符串输入
    bool parse(const std::string& input);
    
    // 获取解析结果
    std::string getResult() const;
    
    // 新增接口
    const std::vector<TokenInfo>& getTokenStream() const;
    void dumpTokens() const; // 输出所有token到日志
private:
    std::string result;  // 存储解析结果
};

#endif // PASCAL_PARSER_H
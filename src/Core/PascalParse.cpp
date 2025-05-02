#include "Core/PascalParse.h"
#include <cstring>
#include "parser.tab.h"  // C 模式下是 parser.h 不是 parser.hpp
#include "AST/visitor.h"
#include <sstream>

// 声明在 lexer.l 和 parser.y 中定义的函数
extern void set_input_string(const char* input);
extern void clear_input_string();
extern void set_parse_result(std::string* result);

extern int yyparse();
extern int yydebug;
extern std::unique_ptr<ASTNode> root;


PascalParser::PascalParser() {
}

PascalParser::~PascalParser() {
}

bool PascalParser::parse(const std::string& input) {
    result.clear();

    set_parse_result(&result);
    set_input_string(input.c_str());

    yydebug = 0; // 启用调试输出（可选）
    int res = yyparse();

    clear_input_string();

    return res == 0;
}

std::string PascalParser::getResult() const {
    return result;
}


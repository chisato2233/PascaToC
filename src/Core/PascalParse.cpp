#include "Core/PascalParse.h"
#include <cstring>
#include "parser.tab.h"  // C 模式下是 parser.h 不是 parser.hpp
#include "Core/Visitors/visitor.h"
#include <sstream>
#include "spdlog/spdlog.h"
#include "Header.h"
#include "ErrorHandler.h"

// 声明在 lexer.l 和 parser.y 中定义的函数
extern void set_input_string(const char* input);
extern void clear_input_string();
extern void set_parse_result(std::string* result);

extern int yyparse();
extern int yydebug;
extern std::unique_ptr<ASTNode> root;

extern const std::vector<TokenInfo>& get_token_stream();
extern const char* get_token_name(int token_type);

PascalParser::PascalParser() {
}

PascalParser::~PascalParser() {
    // 确保不会留下悬空指针
    if (g_error_handler == this) {
        g_error_handler = nullptr;
    }
}

bool PascalParser::parse(const std::string& input) {
    result.clear();

    // 设置当前解析器为错误处理器
    setErrorHandler(this);
    
    set_parse_result(&result);
    set_input_string(input.c_str());

    spdlog::debug("开始解析Pascal源代码...");
    int parse_result = yyparse();
    clear_input_string();
    
    // 解析完成后清理错误处理器
    setErrorHandler(nullptr);
    
    if (parse_result == 0) {
        spdlog::debug("语法分析成功");
        return true;
    } else {
        auto last_token = get_token_stream().back();
        spdlog::error("语法分析失败");
        return false;
    }
}

void PascalParser::logError(const char* message, int line, int column) {
    spdlog::error("错误（在{}行，{}列） - {}", line, column, message);
}

std::string PascalParser::getResult() const {
    return result;
}

const std::vector<TokenInfo>& PascalParser::getTokenStream() const {
    return token_stream;
}

void PascalParser::dumpTokens() const {
    const auto& tokens = get_token_stream();
    spdlog::info("Token流分析结果 (共{}个):", tokens.size());
    
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        std::string value_str;
        
        switch (token.token_type) {
            case INTEGER_CONST:
                value_str = std::to_string(token.value.int_value);
                break;
            case REAL_CONST:
                value_str = std::to_string(token.value.real_value);
                break;
            case STRING_CONST:
            case IDENTIFIER:
                value_str = token.value.str_value ? token.value.str_value : "<NULL>";
                break;
            default:
                value_str = token.token_text;
        }
        
        spdlog::debug("[{}] {} ({}): '{}' 位置={}:{}", 
                    i, 
                    get_token_name(token.token_type), 
                    token.token_type,
                    value_str,
                    token.line, 
                    token.column);
    }
}


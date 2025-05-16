%option noyywrap
%option nodefault
%option nounput
%option noinput
%option caseless

%{
#include "parser.tab.h"

// Token记录结构体
#include "Header.h"
#include <ctype.h>  // 为tolower函数
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line; \
    yylloc.first_column = yylloc.last_column; \
    for(int i = 0; i < yyleng; i++) { \
        if(yytext[i] == '\n') { \
            yylloc.last_line++; \
            yylloc.last_column = 0; \
        } else { \
            yylloc.last_column++; \
        } \
    }

extern YYSTYPE yylval;

// 输入重定向（支持字符串输入）
static char* g_input_string = NULL;
static size_t g_input_string_len = 0;
static size_t g_input_string_pos = 0;

// 记录当前token的辅助函数
void record_token(int token_type, const char* text) {
    TokenInfo info;
    info.token_type = token_type;
    info.token_text = std::string(text);
    info.line = yylloc.first_line;
    info.column = yylloc.first_column;
    
    // 根据token类型保存值
    switch(token_type) {
        case INTEGER_CONST:
            info.value.int_value = yylval.int_val;
            break;
        case REAL_CONST:
            info.value.real_value = yylval.real_val;
            break;
        case STRING_CONST:
        case IDENTIFIER:
            info.value.str_value = yylval.cstr_val;
            break;
    }
    
    token_stream.push_back(info);
    
    // 使用spdlog记录token
    spdlog::debug("Token: {}, 类型: {}, 位置: {}:{}", 
                 text, token_type, yylloc.first_line, yylloc.first_column);
}

int string_input(char* buf, int max_size) {
    int n = 0;
    if (g_input_string != NULL && g_input_string_pos < g_input_string_len) {
        while (n < max_size && g_input_string_pos < g_input_string_len) {
            buf[n++] = g_input_string[g_input_string_pos++];
        }
    }
    return n;
}

#define YY_INPUT(buf, result, max_size) \
    { result = string_input(buf, max_size); }

// 初始化位置信息
void init_lexer_location() {
    yylloc.first_line = yylloc.last_line = 1;
    yylloc.first_column = yylloc.last_column = 0;
    token_stream.clear(); // 清空之前的token流
}

%}

DIGIT       [0-9]
LETTER      [a-zA-Z]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT}|_)*
NUMBER      {DIGIT}+
REAL        {DIGIT}+("."{DIGIT}+)?([eE][+-]?{DIGIT}+)?
CHARACTER   \'(.|\\.)\'
WHITESPACE  [ \t\r\n]+
STRING      \'([^\'\\]|\\.)*\'

%%

{WHITESPACE}    { /* 空白字符，不返回token但会影响位置跟踪 */ }

"{"[^}]*"}"     { 
    /* 处理块注释，需要计算其中的换行 */
    for(int i = 0; i < yyleng; i++) {
        if(yytext[i] == '\n') {
            yylloc.last_line++;
            yylloc.last_column = 0;
        }
    }
}

"//".*          { /* 单行注释，不包含换行 */ }

"program"       { record_token(PROGRAM, yytext); return PROGRAM; }
"begin"         { record_token(BEGIN_TOKEN, yytext); return BEGIN_TOKEN; }
"end"           { record_token(END_TOKEN, yytext); return END_TOKEN; }
"var"           { record_token(VAR, yytext); return VAR; }
"procedure"     { record_token(PROCEDURE, yytext); return PROCEDURE; }
"function"      { record_token(FUNCTION, yytext); return FUNCTION; }
"if"            { record_token(IF, yytext); return IF; }
"then"          { record_token(THEN, yytext); return THEN; }
"else"          { record_token(ELSE, yytext); return ELSE; }
"while"         { record_token(WHILE, yytext); return WHILE; }
"do"            { record_token(DO, yytext); return DO; }
"for"           { record_token(FOR, yytext); return FOR; }
"to"            { record_token(TO, yytext); return TO; }
"downto"        { record_token(DOWNTO, yytext); return DOWNTO; }
"repeat"        { record_token(REPEAT, yytext); return REPEAT; }
"until"         { record_token(UNTIL, yytext); return UNTIL; }
"case"          { record_token(CASE, yytext); return CASE; }
"of"            { record_token(OF, yytext); return OF; }
"const"         { record_token(CONST, yytext); return CONST; }
"type"          { record_token(TYPE, yytext); return TYPE; }
"array"         { record_token(ARRAY, yytext); return ARRAY; }
"record"        { record_token(RECORD, yytext); return RECORD; }
"div"           { record_token(DIV, yytext); return DIV; }
"mod"           { record_token(MOD, yytext); return MOD; }
"and"           { record_token(AND, yytext); return AND; }
"or"            { record_token(OR, yytext); return OR; }
"not"           { record_token(NOT, yytext); return NOT; }
"nil"           { record_token(NIL, yytext); return NIL; }
"write"         { record_token(WRITE, yytext); return WRITE; }
"read"          { record_token(READ, yytext); return READ; }
"Break"         { record_token(BREAK, yytext); return BREAK; } 

"integer"       { record_token(INTEGER, yytext); return INTEGER; }
"real"          { record_token(REAL_TYPE, yytext); return REAL_TYPE; }
"boolean"       { record_token(BOOLEAN, yytext); return BOOLEAN; }
"string"        { record_token(STRING_TYPE, yytext); return STRING_TYPE; }
"char"          { record_token(CHAR, yytext); return CHAR; }

"True"          { yylval.int_val = 1; record_token(BOOL_CONST, yytext); return BOOL_CONST; }
"False"         { yylval.int_val = 0; record_token(BOOL_CONST, yytext); return BOOL_CONST; }

":="            { record_token(ASSIGN, yytext); return ASSIGN; }
"+"             { record_token(PLUS, yytext); return PLUS; }
"-"             { record_token(MINUS, yytext); return MINUS; }
"*"             { record_token(MULTIPLY, yytext); return MULTIPLY; }
"/"             { record_token(DIVIDE, yytext); return DIVIDE; }
"="             { record_token(EQUAL, yytext); return EQUAL; }
"<>"            { record_token(NOT_EQUAL, yytext); return NOT_EQUAL; }
"<"             { record_token(LESS, yytext); return LESS; }
"<="            { record_token(LESS_EQUAL, yytext); return LESS_EQUAL; }
">"             { record_token(GREATER, yytext); return GREATER; }
">="            { record_token(GREATER_EQUAL, yytext); return GREATER_EQUAL; }
"+="            { record_token(PLUS_ASSIGN, yytext); return PLUS_ASSIGN; }
"-="            { record_token(MINUS_ASSIGN, yytext); return MINUS_ASSIGN; }
"*="            { record_token(MULTIPLY_ASSIGN, yytext); return MULTIPLY_ASSIGN; }
"/="            { record_token(DIVIDE_ASSIGN, yytext); return DIVIDE_ASSIGN; }

"("             { record_token(LPAREN, yytext); return LPAREN; }
")"             { record_token(RPAREN, yytext); return RPAREN; }
"["             { record_token(LBRACKET, yytext); return LBRACKET; }
"]"             { record_token(RBRACKET, yytext); return RBRACKET; }
";"             { record_token(SEMICOLON, yytext); return SEMICOLON; }
":"             { record_token(COLON, yytext); return COLON; }
","             { record_token(COMMA, yytext); return COMMA; }
".."            { record_token(DOTDOT, yytext); return DOTDOT; }
"."             { record_token(DOT, yytext); return DOT; }

{NUMBER}        {
                    yylval.int_val = atoi(yytext);
                    record_token(INTEGER_CONST, yytext);
                    return INTEGER_CONST;
                }

{REAL}          {
                    yylval.real_val = atof(yytext);
                    record_token(REAL_CONST, yytext);
                    return REAL_CONST;
                }

{STRING}        {
                    yytext[yyleng - 1] = '\0';  // Remove closing quote
                    yylval.cstr_val = strdup(yytext + 1); // Remove opening quote
                    record_token(STRING_CONST, yytext);
                    return STRING_CONST;
                }

{IDENTIFIER}    {
                    // 创建临时缓冲区
                    char* lowercase = strdup(yytext);
                    // 转换为小写
                    for(int i = 0; lowercase[i]; i++) {
                        lowercase[i] = tolower(lowercase[i]);
                    }
                    yylval.cstr_val = lowercase;  // 使用转换后的小写字符串
                    record_token(IDENTIFIER, yytext);  // 记录原始文本用于调试
                    return IDENTIFIER;
                }

.               {
                    spdlog::error("未知字符: {} 在 {}:{}", yytext, yylloc.first_line, yylloc.first_column);
                    return END_TOKEN; // fallback error token
                }

<<EOF>>         { 
                    spdlog::info("词法分析完成，共识别 {} 个token", token_stream.size()); 
                    return YYEOF; 
                }

%%

// 设置输入字符串接口
void set_input_string(const char* input) {
    if (g_input_string) free(g_input_string);
    g_input_string = strdup(input);
    g_input_string_len = strlen(g_input_string);
    g_input_string_pos = 0;
    
    // 重置位置信息
    init_lexer_location();
    spdlog::debug("开始词法分析，输入长度: {} 字节", g_input_string_len);
}

// 清理输入字符串资源
void clear_input_string() {
    if (g_input_string) free(g_input_string);
    g_input_string = NULL;
    g_input_string_len = 0;
    g_input_string_pos = 0;
}

// 获取当前token流
const std::vector<TokenInfo>& get_token_stream() {
    return token_stream;
}

// 导出token名称的辅助函数 - 用于调试和日志
const char* get_token_name(int token_type) {
    switch(token_type) {
        case PROGRAM: return "PROGRAM";
        case BEGIN_TOKEN: return "BEGIN";
        case END_TOKEN: return "END";
        default: return "UNKNOWN";
    }
}

// 错误报告辅助函数
void report_error(const char* message) {
    spdlog::error("语法错误 (行 {}, 列 {}): {}", 
                 yylloc.first_line, yylloc.first_column, message);
}

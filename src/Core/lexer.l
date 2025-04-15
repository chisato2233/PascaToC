%option noyywrap
%option nodefault
%option nounput
%option noinput

%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

"program"       { return PROGRAM; }
"begin"         { return BEGIN_TOKEN; }
"end"           { return END_TOKEN; }
"var"           { return VAR; }
"procedure"     { return PROCEDURE; }
"function"      { return FUNCTION; }
"if"            { return IF; }
"then"          { return THEN; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"do"            { return DO; }
"for"           { return FOR; }
"to"            { return TO; }
"downto"        { return DOWNTO; }
"repeat"        { return REPEAT; }
"until"         { return UNTIL; }
"case"          { return CASE; }
"of"            { return OF; }
"const"         { return CONST; }
"type"          { return TYPE; }
"array"         { return ARRAY; }
"record"        { return RECORD; }
"div"           { return DIV; }
"mod"           { return MOD; }
"and"           { return AND; }
"or"            { return OR; }
"not"           { return NOT; }
"nil"           { return NIL; }
"write"         { return WRITE; }

"integer"       { return INTEGER; }
"real"          { return REAL_TYPE; }
"boolean"       { return BOOLEAN; }
"string"        { return STRING_TYPE; }
"char"          { return CHAR; }

"true"          { yylval.int_val = 1; return BOOL_CONST; }
"false"         { yylval.int_val = 0; return BOOL_CONST; }

":="            { return ASSIGN; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULTIPLY; }
"/"             { return DIVIDE; }
"="             { return EQUAL; }
"<>"            { return NOT_EQUAL; }
"<"             { return LESS; }
"<="            { return LESS_EQUAL; }
">"             { return GREATER; }
">="            { return GREATER_EQUAL; }
"+="            { return PLUS_ASSIGN; }
"-="            { return MINUS_ASSIGN; }
"*="            { return MULTIPLY_ASSIGN; }
"/="            { return DIVIDE_ASSIGN; }

"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
";"             { return SEMICOLON; }
":"             { return COLON; }
","             { return COMMA; }
".."            { return DOTDOT; }
"."             { return DOT; }

{NUMBER}        {
                    yylval.int_val = atoi(yytext);
                    return INTEGER_CONST;
                }

{REAL}          {
                    yylval.real_val = atof(yytext);
                    return REAL_CONST;
                }

{STRING}        {
                    yytext[yyleng - 1] = '\0';  // Remove closing quote
                    yylval.cstr_val = strdup(yytext + 1); // Remove opening quote
                    return STRING_CONST;
                }

{IDENTIFIER}    {
                    yylval.cstr_val = strdup(yytext);
                    return IDENTIFIER;
                }

.               {
                    fprintf(stderr, "Unknown character: %s\n", yytext);
                    return END_TOKEN; // fallback error token
                }

<<EOF>>         { return YYEOF; }

%%

// 设置位置追踪器
void set_lexer_location(YYLTYPE location) {
    yylloc = location;
}

// 设置输入字符串接口
void set_input_string(const char* input) {
    if (g_input_string) free(g_input_string);
    g_input_string = strdup(input);
    g_input_string_len = strlen(g_input_string);
    g_input_string_pos = 0;
    
    // 重置位置信息
    init_lexer_location();
}

// 清理输入字符串资源
void clear_input_string() {
    if (g_input_string) free(g_input_string);
    g_input_string = NULL;
    g_input_string_len = 0;
    g_input_string_pos = 0;
}

// 获取当前位置信息（用于错误报告）
YYLTYPE get_current_location() {
    return yylloc;
}

// 错误报告辅助函数
void report_error(const char* message) {
    fprintf(stderr, "错误 (行 %d, 列 %d): %s\n", 
            yylloc.first_line, yylloc.first_column, message);
}

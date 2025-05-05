%define parse.error verbose
%define parse.trace
%define api.location.type {SourceLocation} 
%locations

%defines
%code requires {
    #include <string>
    #include <vector>
    #include <iostream>
    #include <cstdlib>
    #include <cstring>
    #include <memory>
    #include <vector>
    #include <string>
    #define YYINITDEPTH  2048      /* 初始栈 */
    #define YYMAXDEPTH  100000     /* 允许的最大深度 */

    #include "../AST/AST.h"
}
%{
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>
#include <string>
#include "../AST/AST.h"

using ExprPtr = std::shared_ptr<Expression>;
using StmtPtr = std::shared_ptr<Statement>;
using DeclPtr = std::shared_ptr<Declaration>;

std::shared_ptr<ASTNode> root;
static std::string* parse_result = nullptr;

extern int yylex();
extern int yylineno;
extern char* yytext;
void yyerror(const char *s);

std::shared_ptr<ASTNode> getAST() {
    return root;
}

void set_parse_result(std::string* result) {
    parse_result = result;
}

// 跟踪当前处理的声明列表
DeclVec* current_declarations = nullptr;

// 保存当前函数参数的列表
std::vector<ParameterInfo>* current_parameters = nullptr;

// 在parser头部添加
std::string current_function_name = "";
%}


%union {
    int int_val;
    double real_val;
    char* cstr_val;
    void* ptr;
    StmtPtr* stmt_ptr;
    ExprPtr* expr_ptr;
    DeclPtr* decl_ptr;
    std::vector<std::string>* str_list;
    ExprVec* expr_list;
    StmtVec* stmt_list;
    DeclVec* decl_list;
    std::vector<int>* range_list;    /* 新增，用来保存数组各维的大小 */
}

%token PROGRAM BEGIN_TOKEN END_TOKEN
%token VAR PROCEDURE FUNCTION
%token IF THEN ELSE
%token WHILE DO FOR TO DOWNTO
%token REPEAT UNTIL
%token BREAK
%token CASE OF
%token CONST TYPE
%token ARRAY RECORD
%token DIV MOD
%token AND OR NOT
%token NIL
%token WRITE READ
%token INTEGER REAL_TYPE BOOLEAN STRING_TYPE CHAR
%token ASSIGN PLUS MINUS MULTIPLY DIVIDE
%token EQUAL NOT_EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL
%token PLUS_ASSIGN MINUS_ASSIGN MULTIPLY_ASSIGN DIVIDE_ASSIGN
%token LPAREN RPAREN LBRACKET RBRACKET
%token SEMICOLON COLON COMMA DOT DOTDOT

%token <int_val> INTEGER_CONST BOOL_CONST
%token <real_val> REAL_CONST
%token <cstr_val> STRING_CONST IDENTIFIER

%type <int_val> direction

%type <stmt_ptr> program_block compound_statement  statement
%type <stmt_ptr> assignment_statement if_statement while_statement
%type <stmt_ptr> repeat_statement for_statement case_statement write_statement read_statement procedure_statement case_element
%type <stmt_list> statement_list
%type <stmt_list> case_element_list

%type <expr_ptr> expression simple_expression term factor function_call constant
%type <expr_list> expression_list

%type <decl_ptr> var_declaration
%type <decl_list> declarations var_declarations var_declaration_list function_declarations procedure_declarations

%type <cstr_val> type simple_type record_type array_type
%type <str_list> identifier_list


%type <int_val> index_range
%type <range_list> index_range_list

%type <expr_ptr> array_expression

%precedence NEG

%start program

%nonassoc THEN_PREC
%nonassoc ELSE

%%

program:
  PROGRAM IDENTIFIER SEMICOLON program_block DOT
  {
    if(parse_result){ (*parse_result)+="sucess\n"; }
    root = std::make_shared<ProgramAST>($2, *((StmtPtr*)$4));
    root->location = @$;
    delete (StmtPtr*)$4;
  }
  ;

program_block:
  declarations compound_statement
  {
    try {
      // 安全检查，确保$1不为空
      if ($1 == nullptr) {
        spdlog::error("program_block中声明列表为空，创建新的");
        $1 = new DeclVec();
      }
      
      $$ = new StmtPtr(std::make_shared<BlockStmt>(*($1), *((StmtPtr*)$2)));
      (*$$)->location = @$;;
      delete $1;
      delete (StmtPtr*)$2;
      
      // 重置current_declarations，避免悬空指针
      current_declarations = nullptr;
    } catch (const std::exception& e) {
      spdlog::error("创建程序块时出错: {}", e.what());
      $$ = new StmtPtr(std::make_shared<BlockStmt>(DeclVec(), 
                       std::make_shared<CompoundStmt>(StmtVec())));
      (*$$)->location = @$;;
    }
  }
  ;

declarations:
  {
    // 创建一个新的声明向量并设为当前
    $$ = new DeclVec();
    current_declarations = $$;
    spdlog::debug("创建新的空声明列表 (地址: {:p})", (void*)$$);
  }
  | const_declarations
  {
    if (current_declarations == nullptr) {
      spdlog::error("从const_declarations返回后current_declarations为空，创建新的");
      current_declarations = new DeclVec();
    }
    $$ = current_declarations;
    spdlog::debug("从const声明获取声明列表 (地址: {:p})，包含{}个项目", 
                  (void*)$$, $$->size());
  }
  | var_declarations
  {
    $$ = $1;
  }
  | procedure_declarations
  {
    $$ = $1;
  }
  | function_declarations
  {
    $$ = $1;
  }
  | declarations const_declarations
  {
    // 保持原来的声明列表
    $$ = $1;
  }
  | declarations type_declarations
  {
    // 修复：添加类型匹配
    $$ = $1;
  }
  | declarations var_declarations
  {
    $$ = $1;
    // 确保$3不为空
    if ($2 != nullptr) {
      for (const auto& decl : *$2) {
        $$->push_back(decl);
      }
      delete $2;
    }
  }
  | declarations procedure_declarations
  {
    $$ = $1;
    // 确保两个列表都存在
    if ($1 && $2) {
      // 将过程声明列表中的内容合并到$1中
      for (const auto& procDecl : *$2) {
        $$->push_back(procDecl);
      }
      spdlog::debug("合并过程声明后，声明列表包含{}个项目", $$->size());
    }
  }
  | declarations function_declarations
  {
    $$ = $1;
    // 确保两个列表都存在
    if ($1 && current_declarations) {
      // 将当前函数声明列表中的内容合并到$1中
      for (const auto& funcDecl : *current_declarations) {
        $$->push_back(funcDecl);
      }
      spdlog::debug("合并函数声明后，声明列表包含{}个项目", $$->size());
    }
  }
  ;

const_declarations:
  CONST const_declaration_list
  {
    // 确保current_declarations不为空
    if (current_declarations == nullptr) {
      spdlog::debug("在const_declarations中创建新的声明列表");
      current_declarations = new DeclVec();
    }
    spdlog::debug("const声明区块处理完成，当前有{}个声明", 
                 current_declarations->size());
    
  }
  ;

const_declaration_list:
  const_declaration
  | const_declaration_list const_declaration
  ;

const_declaration:
  IDENTIFIER EQUAL constant SEMICOLON
  {
    try {
      // 创建常量声明节点
      DeclPtr declPtr = std::make_shared<ConstDeclaration>($1, *((ExprPtr*)$3));
      
      // 将常量加入当前声明列表
      if (current_declarations == nullptr) {
        current_declarations = new DeclVec();
      }
      current_declarations->push_back(declPtr);
      spdlog::debug("添加常量 {} 到声明列表", $1);
      
      // 释放解析过程中分配的内存
      free($1); // 释放IDENTIFIER
      delete (ExprPtr*)$3; // 释放表达式指针
      
    } catch (const std::exception& e) {
      spdlog::error("处理常量声明时出错: {}", e.what());
    }
  }
  ;

type_declarations:
  TYPE type_declaration_list
  ;

type_declaration_list:
  type_declaration
  | type_declaration_list type_declaration
  ;

type_declaration:
  IDENTIFIER EQUAL type SEMICOLON
  ;

var_declarations:
  VAR var_declaration_list
  {
    $$ = $2;
  }
  ;

var_declaration_list:
  var_declaration
  {
    $$ = new DeclVec();
    $$->push_back(*$1);
    delete $1;
  }
  | var_declaration_list var_declaration
  {
    $$ = $1;
    $$->push_back(*$2);
    delete $2;
  }
  ;

// 生成 VarDeclaration 节点，支持普通变量和数组变量
var_declaration:
  identifier_list COLON type SEMICOLON
  {
    $$ = new DeclPtr(std::make_shared<VarDeclaration>(*($1), $3));
    delete $1;
  }
  ;

procedure_declarations:
  procedure_declaration SEMICOLON
  {
    // 返回current_declarations，而不是只检查它
    if (current_declarations == nullptr) {
      spdlog::error("从procedure_declaration返回后current_declarations为空，创建新的");
      current_declarations = new DeclVec();
    }
    $$ = current_declarations; // 添加这行，返回声明列表
    spdlog::debug("过程声明处理完成，当前声明列表包含{}个项目", $$->size());
  }
  ;

procedure_declaration:
  PROCEDURE IDENTIFIER formal_parameters SEMICOLON program_block
  {
    try {
      // 创建过程声明
      auto procBody = *((StmtPtr*)$5);
      std::vector<ParameterInfo> params;
      
      // 使用当前参数列表
      if (current_parameters != nullptr) {
        params = *current_parameters;
        delete current_parameters;
        current_parameters = nullptr;
      } else {
        spdlog::debug("过程{}的参数列表为空", $2);
      }
      
      // 创建过程声明节点
      auto procDecl = std::make_shared<ProcedureDeclaration>($2, params, procBody);
      procDecl->location = @$;
      // 确保current_declarations存在
      if (current_declarations == nullptr) {
        spdlog::debug("创建新的声明列表用于过程{}", $2);
        current_declarations = new DeclVec();
      }
      
      // 添加到当前声明列表
      current_declarations->push_back(procDecl);
      spdlog::debug("添加过程{}到声明列表", $2);
      
      // 释放内存
      free($2);
      delete (StmtPtr*)$5;
    } catch (const std::exception& e) {
      spdlog::error("创建过程声明时出错: {}", e.what());
    }
  }
  ;

function_declarations:
  function_declaration SEMICOLON
  {
    // 返回current_declarations，而不仅仅是检查它
    if (current_declarations == nullptr) {
      current_declarations = new DeclVec();
    }
    $$ = current_declarations;
    spdlog::debug("函数声明处理完成，当前声明列表包含{}个项目", $$->size());
  }
  ;

function_declaration:
  FUNCTION IDENTIFIER formal_parameters COLON type SEMICOLON program_block
  {
    try {
      // 创建函数声明
      current_function_name = $2;
      auto funcBody = *((StmtPtr*)$7);
      std::vector<ParameterInfo> params;
      
      // 使用当前参数列表
      if (current_parameters != nullptr) {
        params = *current_parameters;
        delete current_parameters;
        current_parameters = nullptr;
      } else {
        spdlog::debug("函数{}的参数列表为空", $2);
      }
      
      // 创建函数声明节点
      auto funcDecl = std::make_shared<FunctionDeclaration>($2, $5, params, funcBody);
      funcDecl->location = @$;
      // 确保current_declarations存在
      if (current_declarations == nullptr) {
        spdlog::debug("创建新的声明列表用于函数{}", $2);
        current_declarations = new DeclVec();
      }
      
      // 添加到当前声明列表
      current_declarations->push_back(funcDecl);
      spdlog::debug("添加函数{}到声明列表", $2);
      
      // 释放内存
      free($2);
      free($5);
      delete (StmtPtr*)$7;
    } catch (const std::exception& e) {
      spdlog::error("创建函数声明时出错: {}", e.what());
    }
  }
  ;

formal_parameters:
  /* empty */
  {
    // 创建一个新的参数列表
    current_parameters = new std::vector<ParameterInfo>();
  }
  | LPAREN RPAREN  // 添加这个规则处理空括号
  {
    // 创建一个新的空参数列表
    current_parameters = new std::vector<ParameterInfo>();
  }
  | LPAREN parameter_list RPAREN
  {
    // parameter_list已经填充了current_parameters
  }
  ;

parameter_list:
  parameter_declaration
  | parameter_list SEMICOLON parameter_declaration
  ;

parameter_declaration:
  identifier_list COLON type
  {
    // 检查current_parameters是否存在
    if (current_parameters == nullptr) {
      current_parameters = new std::vector<ParameterInfo>();
      spdlog::debug("在parameter_declaration中创建新的参数列表");
    }
    
    // 添加普通参数（非引用参数）
    for (const auto& name : *$1) {
      current_parameters->push_back(ParameterInfo(name, $3, false));
      spdlog::debug("添加普通参数: {} 类型: {}", name, $3);
    }
    
    delete $1;
    free($3);
  }
  | VAR identifier_list COLON type
  {
    // 检查current_parameters是否存在
    if (current_parameters == nullptr) {
      current_parameters = new std::vector<ParameterInfo>();
      spdlog::debug("在VAR parameter_declaration中创建新的参数列表");
    }
    
    // 添加引用参数（VAR参数）
    for (const auto& name : *$2) {
      current_parameters->push_back(ParameterInfo(name, $4, true));
      spdlog::debug("添加引用参数: {} 类型: {}", name, $4);
    }
    
    delete $2;
    free($4);
  }
  ;

type:
  simple_type
  | array_type
  | record_type
  ;

simple_type:
  INTEGER { $$ = strdup("long long"); }
  | REAL_TYPE { $$ = strdup("double"); }
  | BOOLEAN { $$ = strdup("int"); }
  | STRING_TYPE { $$ = strdup("char*"); }
  | CHAR { $$ = strdup("char"); }
  | IDENTIFIER { $$ = $1; }
  ;

array_type:
  ARRAY LBRACKET index_range_list RBRACKET OF type
  {
    /* $3 为 index_range_list ，$6 为 type 产生式返回的字符串（基类型） */
    std::vector<int>* dims = $3;
    std::ostringstream oss;
    oss << $6;
    // 对每个维度，依次追加 [size]
    for (int size : *dims) {
        oss << "[" << size << "]";
    }
    std::string typeStr = oss.str();
    $$ = strdup(typeStr.c_str());
    free($6);
    delete dims;
  }
  ;

record_type:
  RECORD field_list END_TOKEN
  {
    // 修复：返回一个字符串表示记录类型
    $$ = strdup("record"); // 简单返回record字符串表示
  }
  ;

field_list:
  record_section
  | field_list SEMICOLON record_section
  ;

record_section:
  identifier_list COLON type
  ;

range:
  constant DOTDOT constant
  ;

/* 单个下标范围：例如 1..10，则大小为 10 */
index_range:
    INTEGER_CONST DOTDOT INTEGER_CONST {
        $$ = $3 - $1 + 1;
    }
    ;

/* 下标范围列表：只有一维 */
index_range_list:
    index_range {
        $$ = new std::vector<int>();
        $$->push_back($1);
    }
  | index_range_list COMMA index_range {
        $$ = $1;
        $$->push_back($3);
    }
    ;


constant:
  INTEGER_CONST
  {
    $$ = new ExprPtr(std::make_shared<NumberExpr>($1));
    (*$$)->location = @$;
  }
  | REAL_CONST
  {
    $$ = new ExprPtr(std::make_shared<RealExpr>($1));
    (*$$)->location = @$;
  }
  | STRING_CONST
  {
    $$ = new ExprPtr(std::make_shared<StringExpr>($1));
    (*$$)->location = @$;
  }
  | BOOL_CONST
  {
    $$ = new ExprPtr(std::make_shared<BoolExpr>($1));
    (*$$)->location = @$;
  }
  | IDENTIFIER
  {
    $$ = new ExprPtr(std::make_shared<VariableExpr>($1));
    (*$$)->location = @$;
  }
  ;

compound_statement:
  BEGIN_TOKEN statement_list END_TOKEN
  {
    spdlog::debug("处理完整个复合语句");
    $$ = new StmtPtr(std::make_shared<CompoundStmt>(*(std::vector<std::shared_ptr<Statement>>*)$2));
(*$$)->location = @$;
    delete (std::vector<std::shared_ptr<Statement>>*)$2;
  }
  ;

statement_list:
  /* 空语句列表 */
  {
    spdlog::debug("创建空语句列表");
    $$ = new std::vector<std::shared_ptr<Statement>>();
  }
  | statement
  {
    spdlog::debug("添加第一条语句到语句列表");
    $$ = new std::vector<std::shared_ptr<Statement>>();
    $$->push_back(*((StmtPtr*)$1));
    delete (StmtPtr*)$1;
  }
  | statement_list SEMICOLON
  {
    spdlog::debug("处理语句列表后的分号");
    $$ = $1;
  }
  | statement_list SEMICOLON statement
  {
    spdlog::debug("向语句列表添加新语句");
    $$ = $1;
    $$->push_back(*((StmtPtr*)$3));
    delete (StmtPtr*)$3;
  }
  ;

statement:
  assignment_statement { $$ = $1; }
  | procedure_statement { $$ = $1; }
  | compound_statement { $$ = $1; }
  | if_statement { $$ = $1; }
  | while_statement { $$ = $1; }
  | repeat_statement { $$ = $1; }
  | for_statement { $$ = $1; }
  | case_statement { $$ = $1; }
  | write_statement { $$ = $1; }
  | read_statement { $$ = $1; }
  | BREAK SEMICOLON { 
    $$ = new StmtPtr(std::make_shared<BreakStmt>()); 
    (*$$)->location = @$;
  }
  | /* 空语句 */ 
  {
    // 创建一个空语句节点
    $$ = new StmtPtr(std::make_shared<EmptyStmt>());
  }
  ;

assignment_statement:
  IDENTIFIER ASSIGN expression
  {
    // 处理没有分号的情况
    if ($1 == current_function_name) {
      $$ = new StmtPtr(std::make_shared<ReturnStmt>(*((ExprPtr*)$3)));
  (*$$)->location = @$;
    } else {
      $$ = new StmtPtr(std::make_shared<AssignmentStmt>($1, *((ExprPtr*)$3)));
  (*$$)->location = @$;
    }
    free($1);
    delete (ExprPtr*)$3;
  }
  | array_expression ASSIGN expression
  {
    // 数组赋值
    auto arrayAccess = std::dynamic_pointer_cast<ArrayAccessExpr>(*((ExprPtr*)$1));
    if (arrayAccess) {
      $$ = new StmtPtr(std::make_shared<ArrayAssignmentStmt>(arrayAccess, *((ExprPtr*)$3)));
  (*$$)->location = @$;
    } else {
      // 创建错误语句或抛出异常
      spdlog::error("无法将表达式转换为数组访问表达式");
      $$ = new StmtPtr(std::make_shared<EmptyStmt>());
  (*$$)->location = @$;
    }
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }


if_statement:
  IF expression THEN statement
  {
    $$ = new StmtPtr(std::make_shared<IfStmt>(*((ExprPtr*)$2), *((StmtPtr*)$4), nullptr));
(*$$)->location = @$;
    delete (ExprPtr*)$2;
    delete (StmtPtr*)$4;
  }
  | IF expression THEN statement ELSE statement
  {
    $$ = new StmtPtr(std::make_shared<IfStmt>(*((ExprPtr*)$2), *((StmtPtr*)$4), *((StmtPtr*)$6)));
(*$$)->location = @$;
    delete (ExprPtr*)$2;
    delete (StmtPtr*)$4;
    delete (StmtPtr*)$6;
  }
  ;

while_statement:
  WHILE expression DO statement
  {
    $$ = new StmtPtr(std::make_shared<WhileStmt>(*((ExprPtr*)$2), *((StmtPtr*)$4)));
(*$$)->location = @$;
    delete (ExprPtr*)$2;
    delete (StmtPtr*)$4;
  }
  ;

repeat_statement:
  REPEAT statement_list UNTIL expression
  {
    $$ = new StmtPtr(std::make_shared<RepeatStmt>(*(std::vector<std::shared_ptr<Statement>>*)$2, *((ExprPtr*)$4)));
(*$$)->location = @$;
    delete (std::vector<std::shared_ptr<Statement>>*)$2;
    delete (ExprPtr*)$4;
  }
  ;

for_statement:
  FOR IDENTIFIER ASSIGN expression direction expression DO statement
  {
    $$ = new StmtPtr(std::make_shared<ForStmt>($2, *((ExprPtr*)$4), $5, *((ExprPtr*)$6), *((StmtPtr*)$8)));
(*$$)->location = @$;
    free($2);
    delete (ExprPtr*)$4;
    delete (ExprPtr*)$6;
    delete (StmtPtr*)$8;
  }
  ;

direction:
  TO    { $$ = +1; }
  | DOWNTO { $$ = -1; }
  ;

case_statement:
  CASE expression OF case_element_list END_TOKEN
  {
    $$ = new StmtPtr(std::make_shared<CaseStmt>(*((ExprPtr*)$2), *(std::vector<std::shared_ptr<Statement>>*)$4));
    (*$$)->location = @$;
    delete (ExprPtr*)$2;
    delete (std::vector<std::shared_ptr<Statement>>*)$4;
  }
  ;

case_element_list:
  case_element
  {
    $$ = new std::vector<std::shared_ptr<Statement>>();
    $$->push_back(*((StmtPtr*)$1));
    delete (StmtPtr*)$1;
  }
  | case_element_list SEMICOLON case_element
  {
    $$ = $1;
    $$->push_back(*((StmtPtr*)$3));
    delete (StmtPtr*)$3;
  }
  ;

case_element:
  constant COLON statement
  {
    $$ = new StmtPtr(std::make_shared<CaseElementStmt>(*((ExprPtr*)$1), *((StmtPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (StmtPtr*)$3;
  }
  ;

write_statement:
  WRITE LPAREN expression_list RPAREN
  {
    $$ = new StmtPtr(std::dynamic_pointer_cast<Statement>(std::make_shared<WriteStmt>(*$3)));
(*$$)->location = @$;
    delete (ExprVec*)$3;
  }
  ;

read_statement:
  READ LPAREN expression_list RPAREN
  {
    $$ = new StmtPtr(std::dynamic_pointer_cast<Statement>(std::make_shared<ReadStmt>(*$3)));
(*$$)->location = @$;
    delete (ExprVec*)$3;
  }
  ;
expression_list:
  expression
  {
    $$ = new ExprVec();
    $$->push_back(*$1);

    delete $1;
  }
  | expression_list COMMA expression
  {
    $$ = $1;
    $$->push_back(*$3);

    delete $3;
  }
  ;

expression:
    simple_expression
  {
    $$ = $1;
(*$$)->location = @$;
  }
  | simple_expression EQUAL simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Equal, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression NOT_EQUAL simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::NotEqual, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression LESS simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Less, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression GREATER simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Greater, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression LESS_EQUAL simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::LessEqual, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression GREATER_EQUAL simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::GreaterEqual, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
;

simple_expression:
    term
  {
    $$ = $1;
  }
  | simple_expression PLUS term
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Plus, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | PLUS term %prec NEG
  {
    $$ = $2; // 处理+term的情况
  }
  | simple_expression MINUS term
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Minus, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | MINUS term %prec NEG
  {
    $$ = new ExprPtr(std::make_shared<UnaryExpr>(UnaryExpr::Neg, *((ExprPtr*)$2)));
(*$$)->location = @$;
    delete (ExprPtr*)$2;
  }
  | simple_expression OR term
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Or, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
;

term:
    factor
  {
    $$ = $1;
(*$$)->location = @$;
  }
  | term MULTIPLY factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Multiply, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | term DIVIDE factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Divide, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | term DIV factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Div, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | term MOD factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Mod, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | term AND factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::And, *((ExprPtr*)$1), *((ExprPtr*)$3)));
(*$$)->location = @$;
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
;

factor:
    INTEGER_CONST
  {
    $$ = new ExprPtr(std::make_shared<NumberExpr>($1));
(*$$)->location = @$;
  }
  | MINUS INTEGER_CONST %prec NEG
  {
    $$ = new ExprPtr(std::make_shared<NumberExpr>(-$2));
(*$$)->location = @$;
  }
  | PLUS INTEGER_CONST %prec NEG
  {
    $$ = new ExprPtr(std::make_shared<NumberExpr>($2));
(*$$)->location = @$;
  }
  | MINUS factor %prec NEG
  {
    $$ = new ExprPtr(std::make_shared<UnaryExpr>(UnaryExpr::Neg, *((ExprPtr*)$2)));
(*$$)->location = @$;
    delete (ExprPtr*)$2;
  }
  | PLUS factor %prec NEG
  {
    $$ = $2; // 正号(+)不改变表达式的值
(*$$)->location = @$;
  }
  | REAL_CONST
  {
    $$ = new ExprPtr(std::make_shared<RealExpr>($1));
(*$$)->location = @$;
  }
  | BOOL_CONST
  {
    $$ = new ExprPtr(std::make_shared<BoolExpr>($1));
(*$$)->location = @$;
  }
  | STRING_CONST
  {
    $$ = new ExprPtr(std::make_shared<StringExpr>($1));
(*$$)->location = @$;
  }
  | IDENTIFIER
  {
    $$ = new ExprPtr(std::make_shared<VariableExpr>($1));
(*$$)->location = @$;
  }
  | LPAREN expression RPAREN
  {
    $$ = $2;
(*$$)->location = @$;
  }
  | NOT factor
  {
    $$ = new ExprPtr(std::make_shared<UnaryExpr>(UnaryExpr::Not, *((ExprPtr*)$2)));
(*$$)->location = @$;
    delete (ExprPtr*)$2;
  }
  | function_call
  {
    $$ = $1;
(*$$)->location = @$;
  }
  | array_expression
  {
    $$ = $1;
(*$$)->location = @$;
  }
;

function_call:
  IDENTIFIER LPAREN expression_list RPAREN
  {
    $$ = new ExprPtr(std::make_shared<FunctionCall>($1, *(std::vector<std::shared_ptr<Expression>>*)$3));
(*$$)->location = @$;
    delete (std::vector<std::shared_ptr<Expression>>*)$3;
  }
  | IDENTIFIER LPAREN RPAREN  // 添加处理空参数列表的情况
  {
    $$ = new ExprPtr(std::make_shared<FunctionCall>($1, ExprVec()));
(*$$)->location = @$;
    free($1);
  }
  ;

identifier_list:
  IDENTIFIER
  {
    $$ = new std::vector<std::string>();
    $$->push_back($1);
  }
  | identifier_list COMMA IDENTIFIER
  {
    $$ = $1;
    $$->push_back($3);
  }
  ;

procedure_statement:
  IDENTIFIER
  {
    $$ = new StmtPtr(std::make_shared<FunctionCallStmt>($1, ExprVec()));
(*$$)->location = @$;
    free($1);
  }
  | IDENTIFIER LPAREN expression_list RPAREN
  {
    $$ = new StmtPtr(std::make_shared<FunctionCallStmt>($1, *$3));
(*$$)->location = @$;
    free($1);
    delete $3;
  }
  | IDENTIFIER LPAREN RPAREN
  {
    $$ = new StmtPtr(std::make_shared<FunctionCallStmt>($1, ExprVec()));
(*$$)->location = @$;
    free($1);
  }
  ;

array_expression:
  IDENTIFIER LBRACKET expression RBRACKET
  {
    $$ = new ExprPtr(std::make_shared<ArrayAccessExpr>($1, *((ExprPtr*)$3)));
(*$$)->location = @$;
    free($1);
    delete (ExprPtr*)$3;
  }
  | IDENTIFIER LBRACKET expression_list RBRACKET
  {
    // 支持多维数组访问
    $$ = new ExprPtr(std::make_shared<ArrayAccessExpr>($1, *$3));
(*$$)->location = @$;
    free($1);
    delete $3;
  }
  | array_expression LBRACKET expression RBRACKET
  {
    // 支持嵌套数组访问，例如a[i][j]
    auto prevAccess = std::dynamic_pointer_cast<ArrayAccessExpr>(*((ExprPtr*)$1));
    if (prevAccess) {
      // 创建新的嵌套访问
      $$ = new ExprPtr(std::make_shared<ArrayAccessExpr>(*((ExprPtr*)$1), *((ExprPtr*)$3)));
  (*$$)->location = @$;
    } else {
      spdlog::error("嵌套数组访问的基础不是数组");
      $$ = new ExprPtr(std::make_shared<ArrayAccessExpr>("error", *((ExprPtr*)$3)));
  (*$$)->location = @$;
    }
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  ;

%%

void yyerror(const char*s){
  std::cerr << "Syntax Error: " << s << std::endl;
  if(parse_result){
    (*parse_result) += "error: ";
    (*parse_result) += s;
    (*parse_result) += "\n";
  }
}

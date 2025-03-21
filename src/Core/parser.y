%define parse.error verbose
%define parse.trace
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

std::unique_ptr<ASTNode> root;
static std::string* parse_result = nullptr;

extern int yylex();
extern int yylineno;
extern char* yytext;
void yyerror(const char *s);

ASTNode& getAST() {
    return *root;
}

void set_parse_result(std::string* result) {
    parse_result = result;
}
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
}

%token PROGRAM BEGIN_TOKEN END_TOKEN
%token VAR PROCEDURE FUNCTION
%token IF THEN ELSE
%token WHILE DO FOR TO DOWNTO
%token REPEAT UNTIL
%token CASE OF
%token CONST TYPE
%token ARRAY RECORD
%token DIV MOD
%token AND OR NOT
%token NIL
%token WRITE
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
%type <stmt_ptr> assignment_statement procedure_statement if_statement while_statement
%type <stmt_ptr> repeat_statement for_statement case_statement write_statement
%type <stmt_list> statement_list
%type <stmt_list> case_element_list

%type <expr_ptr> expression simple_expression term factor function_call
%type <expr_list> expression_list

%type <decl_ptr> var_declaration
%type <decl_list> declarations var_declarations var_declaration_list

%type <cstr_val> type simple_type record_type array_type
%type <str_list> identifier_list

%type <ptr> case_element constant 

%start program

%%

program:
  PROGRAM IDENTIFIER SEMICOLON program_block DOT
  {
    if(parse_result){(*parse_result)+="sucess\n";}
    root=std::make_unique<ProgramAST>($2,*((StmtPtr*)$4));
    delete (StmtPtr*)$4;
  }
  ;

program_block:
  declarations compound_statement
  {
    $$=new StmtPtr(std::make_shared<BlockStmt>(*($1),*((StmtPtr*)$2)));
    delete $1;
    delete (StmtPtr*)$2;
  }
  ;

declarations:
  const_declarations
  | type_declarations
  | var_declarations
  | procedure_declarations
  | function_declarations
  | declarations const_declarations
  | declarations type_declarations
  | declarations var_declarations
  | declarations procedure_declarations
  | declarations function_declarations
  ;

const_declarations:
  CONST const_declaration_list
  ;

const_declaration_list:
  const_declaration
  | const_declaration_list const_declaration
  ;

const_declaration:
  IDENTIFIER EQUAL constant SEMICOLON
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
    $$=$2;
  }
  ;

var_declaration_list:
  var_declaration
  {
    $$=new DeclVec();
    $$->push_back(*$1);
    delete $1;
  }
  | var_declaration_list var_declaration
  {
    $$=$1;
    $$->push_back(*$2);
    delete $2;
  }
  ;

var_declaration:
  identifier_list COLON type SEMICOLON
  {
    $$=new DeclPtr(std::make_shared<VarDeclaration>(*($1),$3));
    delete $1;
  }
  ;

procedure_declarations:
  procedure_declaration SEMICOLON
  ;

procedure_declaration:
  PROCEDURE IDENTIFIER formal_parameters SEMICOLON program_block
  ;

function_declarations:
  function_declaration SEMICOLON
  ;

function_declaration:
  FUNCTION IDENTIFIER formal_parameters COLON type SEMICOLON program_block
  ;

formal_parameters:
  /* empty */
  | LPAREN parameter_list RPAREN
  ;

parameter_list:
  parameter_declaration
  | parameter_list SEMICOLON parameter_declaration
  ;

parameter_declaration:
  identifier_list COLON type
  | VAR identifier_list COLON type
  ;

type:
  simple_type
  | array_type
  | record_type
  ;

simple_type:
  INTEGER {$$=strdup("int");}
  | REAL_TYPE {$$=strdup("double");}
  | BOOLEAN {$$=strdup("bool");}
  | STRING_TYPE {$$=strdup("char*");}
  | CHAR {$$=strdup("char");}
  | IDENTIFIER {$$=$1;}
  ;

array_type:
  ARRAY LBRACKET range RBRACKET OF type
  ;

record_type:
  RECORD field_list END_TOKEN
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

constant:
  INTEGER_CONST
  | REAL_CONST
  | STRING_CONST
  | BOOL_CONST
  | IDENTIFIER
  ;

compound_statement:
  BEGIN_TOKEN statement_list END_TOKEN
  {
    $$=new StmtPtr(std::make_shared<CompoundStmt>(*(std::vector<std::shared_ptr<Statement>>*)$2));
    delete (std::vector<std::shared_ptr<Statement>>*)$2;
  }
  ;

statement_list:
  statement
  {
    $$=new std::vector<std::shared_ptr<Statement>>();
    $$->push_back(*((StmtPtr*)$1));
    delete (StmtPtr*)$1;
  }
  | statement_list statement
  {
    $$=$1;
    $$->push_back(*((StmtPtr*)$2));
    delete (StmtPtr*)$2;
  }
  ;

statement:
  assignment_statement {$$=$1;}
  | procedure_statement {$$=$1;}
  | compound_statement {$$=$1;}
  | if_statement {$$=$1;}
  | while_statement {$$=$1;}
  | repeat_statement {$$=$1;}
  | for_statement {$$=$1;}
  | case_statement {$$=$1;}
  | write_statement {$$=$1;}
  ;

assignment_statement:
  IDENTIFIER ASSIGN expression SEMICOLON
  {
    $$=new StmtPtr(std::make_shared<AssignmentStmt>($1,*((ExprPtr*)$3)));
    delete (ExprPtr*)$3;
  }
  ;

procedure_statement:
  IDENTIFIER
  | IDENTIFIER LPAREN expression_list RPAREN
  ;

if_statement:
  IF expression THEN statement
  {
    $$=new StmtPtr(std::make_shared<IfStmt>(*((ExprPtr*)$2),*((StmtPtr*)$4),nullptr));
    delete (ExprPtr*)$2;
    delete (StmtPtr*)$4;
  }
  | IF expression THEN statement ELSE statement
  {
    $$=new StmtPtr(std::make_shared<IfStmt>(*((ExprPtr*)$2),*((StmtPtr*)$4),*((StmtPtr*)$6)));
    delete (ExprPtr*)$2;
    delete (StmtPtr*)$4;
    delete (StmtPtr*)$6;
  }
  ;

while_statement:
  WHILE expression DO statement
  {
    $$=new StmtPtr(std::make_shared<WhileStmt>(*((ExprPtr*)$2),*((StmtPtr*)$4)));
    delete (ExprPtr*)$2;
    delete (StmtPtr*)$4;
  }
  ;

repeat_statement:
  REPEAT statement_list UNTIL expression
  {
    $$=new StmtPtr(std::make_shared<RepeatStmt>(*(std::vector<std::shared_ptr<Statement>>*)$2,*((ExprPtr*)$4)));
    delete (std::vector<std::shared_ptr<Statement>>*)$2;
    delete (ExprPtr*)$4;
  }
  ;

for_statement:
  FOR IDENTIFIER ASSIGN expression direction expression DO statement
  {
    $$=new StmtPtr(std::make_shared<ForStmt>($2,*((ExprPtr*)$4),$5,*((ExprPtr*)$6),*((StmtPtr*)$8)));
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
    $$=new StmtPtr(std::make_shared<CaseStmt>(*((ExprPtr*)$2),*(std::vector<std::shared_ptr<Statement>>*)$4));
    delete (ExprPtr*)$2;
    delete (std::vector<std::shared_ptr<Statement>>*)$4;
  }
  ;

case_element_list:
  case_element
  {
    $$=new std::vector<std::shared_ptr<Statement>>();
    $$->push_back(*((StmtPtr*)$1));
    delete (StmtPtr*)$1;
  }
  | case_element_list SEMICOLON case_element
  {
    $$=$1;
    $$->push_back(*((StmtPtr*)$3));
    delete (StmtPtr*)$3;
  }
  ;

case_element:
  constant COLON statement
  {
    $$=new StmtPtr(std::make_shared<CaseElementStmt>(*((ExprPtr*)$1),*((StmtPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (StmtPtr*)$3;
  }
  ;

write_statement:
  WRITE LPAREN expression RPAREN SEMICOLON
  {
    $$=new StmtPtr(std::dynamic_pointer_cast<Statement>(std::make_shared<WriteStmt>(*$3)));
    delete (ExprPtr*)$3;
  }
  ;

expression_list:
  expression
  {
    $$=new ExprVec();
    $$->push_back(*$1);
    delete $1;
  }
  | expression_list COMMA expression
  {
    $$=$1;
    $$->push_back(*$3);
    delete $3;
  }
  ;
expression:
    simple_expression
  {
    $$ = $1;
  }
  | simple_expression EQUAL simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Equal, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression NOT_EQUAL simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::NotEqual, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression LESS simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Less, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression GREATER simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Greater, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression LESS_EQUAL simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::LessEqual, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression GREATER_EQUAL simple_expression
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::GreaterEqual, *((ExprPtr*)$1), *((ExprPtr*)$3)));
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
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression MINUS term
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Minus, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | simple_expression OR term
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Or, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
;

term:
    factor
  {
    $$ = $1;
  }
  | term MULTIPLY factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Multiply, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | term DIVIDE factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Divide, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | term DIV factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Div, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | term MOD factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::Mod, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
  | term AND factor
  {
    $$ = new ExprPtr(std::make_shared<BinaryExpr>(BinaryExpr::And, *((ExprPtr*)$1), *((ExprPtr*)$3)));
    delete (ExprPtr*)$1;
    delete (ExprPtr*)$3;
  }
;

factor:
    INTEGER_CONST
  {
    $$ = new ExprPtr(std::make_shared<NumberExpr>($1));
  }
  | REAL_CONST
  {
    $$ = new ExprPtr(std::make_shared<RealExpr>($1));
  }
  | BOOL_CONST
  {
    $$ = new ExprPtr(std::make_shared<BoolExpr>($1));
  }
  | STRING_CONST
  {
    $$ = new ExprPtr(std::make_shared<StringExpr>($1));
  }
  | IDENTIFIER
  {
    $$ = new ExprPtr(std::make_shared<VariableExpr>($1));
  }
  | LPAREN expression RPAREN
  {
    $$ = $2;
  }
  | NOT factor
  {
    $$ = new ExprPtr(std::make_shared<UnaryExpr>(UnaryExpr::Not, *((ExprPtr*)$2)));
    delete (ExprPtr*)$2;
  }
  | function_call
  {
    $$ = $1;
  }
;

function_call:
  IDENTIFIER LPAREN expression_list RPAREN
  {
    $$=new ExprPtr(std::make_shared<FunctionCall>($1,*(std::vector<std::shared_ptr<Expression>>*)$3));
    delete (std::vector<std::shared_ptr<Expression>>*)$3;
  }
  ;

identifier_list:
  IDENTIFIER
  {
    $$=new std::vector<std::string>();
    $$->push_back($1);
  }
  | identifier_list COMMA IDENTIFIER
  {
    $$=$1;
    $$->push_back($3);
  }
  ;

%%

void yyerror(const char*s){
  std::cerr<<"Syntax Error: "<<s<<std::endl;
  if(parse_result){
    (*parse_result)+="error: ";
    (*parse_result)+=s;
    (*parse_result)+="\n";
  }
}

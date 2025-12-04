/* parser.y - SysY 语法分析器 */

%debug
%glr-parser

%union {
    int ival;
    double fval;
    char *ident;
}

/* ===== C/C++ 代码区（只进 .c/.cpp，不进头文件） ===== */
%{
#include <stdio.h>
#include <stdlib.h>
#include "flex_bison_config.h"
#include "error_handler.hpp"  
%}

/* 错误信息更友好一点 */
%define parse.error verbose

/* 处理悬空 else 的优先级声明 */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* ====== token 声明，对应你的 flex_input.l ====== */

/* 关键字 */
%token CONST INT FLOAT VOID
%token IF ELSE WHILE BREAK CONTINUE RETURN

/* 比较、逻辑运算符 */
%token EQ NEQ LE GE AND OR

/* 算术运算符 */
%token PLUS MINUS MUL DIV MOD

/* 其他运算符/分隔符 */
%token LT GT ASSIGN NOT
%token SEMICOLON COMMA
%token LPARENT RPARENT
%token LBRACK RBRACK
%token LBRACE RBRACE

/* 常量与标识符（你的 .l 已经给 yylval.ival / fval / ident 赋值） */
%token INT_CONST
%token FLOAT_CONST
%token IDENT

/* 一些错误 token（可以先不在文法中用） */
%token BAD_OCT BAD_HEX BAD_FLOAT BAD_IDENT UNKNOWN_CHAR

/* 文法开始符号 */
%start CompUnit

%%  /* ================== 语法规则区 ================== */

/* TODO: 捕捉错误信息 */

/* 编译单元 CompUnit → [ CompUnit ] ( Decl | FuncDef )
 * 这里展开成一个列表
 */
CompUnit
    : CompUnitItems
    ;

CompUnitItems
    : /* empty */
    | CompUnitItems CompUnitItem
    ;

/* 关键：顶层先匹配函数定义，再匹配声明，避免 int main() 被当成 VarDecl */
CompUnitItem
    : FuncDef
    | Decl
    ;

/* 声明 Decl → ConstDecl | VarDecl */
Decl
    : ConstDecl
    | VarDecl
    ;

/* 常量声明 ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';' */
ConstDecl
    : CONST BType ConstDefList SEMICOLON
    ;

ConstDefList
    : ConstDef
    | ConstDefList COMMA ConstDef
    ;

/* ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal */
ConstDef
    : IDENT ConstDimList ASSIGN ConstInitVal
    ;

ConstDimList
    : /* empty */
    | ConstDimList LBRACK ConstExp RBRACK
    ;

/* ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}' */
ConstInitVal
    : ConstExp
    | LBRACE ConstInitValListOpt RBRACE
    ;

ConstInitValListOpt
    : /* empty */
    | ConstInitValList
    ;

ConstInitValList
    : ConstInitVal
    | ConstInitValList COMMA ConstInitVal
    ;

/* 基本类型 BType → 'int' | 'float' */
BType
    : INT
    | FLOAT
    ;

/* 变量声明 VarDecl → BType VarDef { ',' VarDef } ';' */
VarDecl
    : BType VarDefList SEMICOLON
    ;

VarDefList
    : VarDef
    | VarDefList COMMA VarDef
    ;

/* VarDef → Ident { '[' ConstExp ']' }
 *        | Ident { '[' ConstExp ']' } '=' InitVal
 */
VarDef
    : IDENT VarDimList
    | IDENT VarDimList ASSIGN InitVal
    ;

VarDimList
    : /* empty */
    | VarDimList LBRACK ConstExp RBRACK
    ;

/* InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}' */
InitVal
    : Exp
    | LBRACE InitValListOpt RBRACE
    ;

InitValListOpt
    : /* empty */
    | InitValList
    ;

InitValList
    : InitVal
    | InitValList COMMA InitVal
    ;

/* 函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block */
FuncDef
    : FuncType IDENT LPARENT FuncFParamsOpt RPARENT Block
    ;

FuncType
    : VOID
    | INT
    | FLOAT
    ;

FuncFParamsOpt
    : /* empty */
    | FuncFParams
    ;

/* FuncFParams → FuncFParam { ',' FuncFParam } */
FuncFParams
    : FuncFParam
    | FuncFParams COMMA FuncFParam
    ;

/* FuncFParam → BType Ident ['[' ']' { '[' Exp ']' }] */
FuncFParam
    : BType IDENT
    | BType IDENT LBRACK RBRACK FuncFParamDimsOpt
    ;

FuncFParamDimsOpt
    : /* empty */
    | FuncFParamDimsOpt LBRACK Exp RBRACK
    ;

/* Block → '{' { BlockItem } '}' */
Block
    : LBRACE BlockItemListOpt RBRACE
    ;

BlockItemListOpt
    : /* empty */
    | BlockItemList
    ;

BlockItemList
    : BlockItem
    | BlockItemList BlockItem
    ;

/* BlockItem → Decl | Stmt */
BlockItem
    : Decl
    | Stmt
    ;

/* Stmt 各种形式 */
Stmt
    /* 赋值语句：LVal '=' Exp ';' */
    : LVal ASSIGN Exp SEMICOLON

    /* 表达式语句：[Exp] ';' */
    | ExpOpt SEMICOLON

    /* 语句块 */
    | Block

    /* if (Cond) Stmt [else Stmt] */
    | IF LPARENT Cond RPARENT Stmt %prec LOWER_THAN_ELSE
    | IF LPARENT Cond RPARENT Stmt ELSE Stmt

    /* while (Cond) Stmt */
    | WHILE LPARENT Cond RPARENT Stmt

    /* break; */
    | BREAK SEMICOLON

    /* continue; */
    | CONTINUE SEMICOLON

    /* return [Exp] ';' */
    | RETURN ExpOpt SEMICOLON
    ;

ExpOpt
    : /* empty */
    | Exp
    ;

/* 表达式 Exp → AddExp */
Exp
    : AddExp
    ;

/* 条件表达式 Cond → LOrExp */
Cond
    : LOrExp
    ;

/* LVal → Ident {'[' Exp ']'} */
LVal
    : IDENT LValDimsOpt
    ;

LValDimsOpt
    : /* empty */
    | LValDimsOpt LBRACK Exp RBRACK
    ;

/* PrimaryExp → '(' Exp ')' | LVal | Number */
PrimaryExp
    : LPARENT Exp RPARENT
    | LVal
    | Number
    ;

/* Number → IntConst | FloatConst */
Number
    : INT_CONST
    | FLOAT_CONST
    ;

/* UnaryExp → PrimaryExp
 *          | Ident '(' [FuncRParams] ')'
 *          | UnaryOp UnaryExp
 */
UnaryExp
    : PrimaryExp
    | IDENT LPARENT FuncRParamsOpt RPARENT
    | UnaryOp UnaryExp
    ;

/* UnaryOp → '+' | '−' | '!' */
UnaryOp
    : PLUS  %prec UPLUS
    | MINUS %prec UMINUS
    | NOT
    ;

/* FuncRParams → Exp { ',' Exp } */
FuncRParamsOpt
    : /* empty */
    | FuncRParams
    ;

FuncRParams
    : Exp
    | FuncRParams COMMA Exp
    ;

/* MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp */
MulExp
    : UnaryExp
    | MulExp MUL UnaryExp
    | MulExp DIV UnaryExp
    | MulExp MOD UnaryExp
    ;

/* AddExp → MulExp | AddExp ('+' | '−') MulExp */
AddExp
    : MulExp
    | AddExp PLUS MulExp
    | AddExp MINUS MulExp
    ;

/* RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp */
RelExp
    : AddExp
    | RelExp LT AddExp
    | RelExp GT AddExp
    | RelExp LE AddExp
    | RelExp GE AddExp
    ;

/* EqExp → RelExp | EqExp ('==' | '!=') RelExp */
EqExp
    : RelExp
    | EqExp EQ RelExp
    | EqExp NEQ RelExp
    ;

/* LAndExp → EqExp | LAndExp '&&' EqExp */
LAndExp
    : EqExp
    | LAndExp AND EqExp
    ;

/* LOrExp → LAndExp | LOrExp '||' LAndExp */
LOrExp
    : LAndExp
    | LOrExp OR LAndExp
    ;

/* 常量表达式 ConstExp → AddExp */
ConstExp
    : AddExp
    ;

%%  /* ================== 代码区 ================== */

void yyerror(const char *msg) {
    bison_error_handler();
}


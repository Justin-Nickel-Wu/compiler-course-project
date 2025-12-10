/* parser.y - SysY 语法分析器 */

%debug
%glr-parser
%locations

%union {
    // 节点编号
    int node_id;
}

/* ===== C/C++ 代码区（只进 .c/.cpp，不进头文件） ===== */
%{
#include <stdio.h>
#include <stdlib.h>
#include "flex_bison_config.hpp"
#include "error_handler.hpp"  
#include "parse_tree.hpp"
%}

/* 错误信息更友好一点 */
%define parse.error custom

/* 处理悬空 else 的优先级声明 */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* ====== token 声明，对应你的 flex_input.l ====== */

/* 关键字 */
%token <node_id> CONST INT FLOAT VOID
%token <node_id> IF ELSE WHILE BREAK CONTINUE RETURN

/* 比较、逻辑运算符 */
%token <node_id> EQ NEQ LE GE AND OR

/* 算术运算符 */
%token <node_id> PLUS "+" MINUS "-" MUL "*" DIV "/" MOD "%"

/* 其他运算符/分隔符 */
%token <node_id> LT GT ASSIGN NOT
%token <node_id> SEMICOLON ";" COMMA ","
%token <node_id> LPARENT "(" RPARENT ")"
%token <node_id> LBRACK "[" RBRACK "]"
%token <node_id> LBRACE "{" RBRACE "}"

/* 常量与标识符（你的 .l 已经给 yylval.ival / fval / ident 赋值） */
%token <node_id> INT_CONST
%token <node_id> FLOAT_CONST
%token <node_id> IDENT

/* 一些错误 token（bison中未使用，用于flex识别错误） */
%token <node_id> BAD_OCT BAD_HEX BAD_FLOAT BAD_IDENT UNKNOWN_CHAR

/* 文法开始符号 */
%start CompUnits

%type <node_id> CompUnits CompUnit Decl ConstDecl ConstDefList ConstDef ConstInitVal  ConstInitValList
%type <node_id> BType VarDecl VarDefList VarDef InitVal FuncDef FuncType InitValList
%type <node_id> FuncFParams FuncFParam Block BlockItem BlockItemList Stmt FuncFParamDimsList
%type <node_id> Exp Cond LVal PrimaryExp Number UnaryExp UnaryOp VarDimList ConstDimList
%type <node_id> FuncRParams MulExp AddExp RelExp EqExp LAndExp LOrExp ConstExp LValDimsList


%%  /* ================== 语法规则区 ================== */
/* CompUnit → [CompUnit](Decl | FuncDef) */
CompUnits
    : CompUnit CompUnits { 
        $$ = make_node("CompUnits", -1, -1, $1, $2);
        set_root($$);
    }
    | CompUnit {
        $$ = make_node("CompUnits", -1, -1, $1);
        set_root($$);
    }

CompUnit
    : FuncDef {
        $$ = make_node("CompUnit", -1, -1, $1);
    }
    | Decl {
        $$ = make_node("CompUnit", -1, -1, $1);
    }
    ;

/* Decl → ConstDecl | VarDecl */
Decl
    : ConstDecl {
        $$ = make_node("Decl", -1, -1, $1); 
    }
    | VarDecl {
        $$ = make_node("Decl", -1, -1, $1); 
    }
    ;

/* ConstDecl → 'const' BType ConstDefList ';' */
ConstDecl
    : CONST BType ConstDefList SEMICOLON {
        $$ = make_node("ConstDecl", -1, -1, $1, $2, $3, $4);
    }
    ;

/* ConstDefList → ConstDef { ',' ConstDef } */
ConstDefList
    : ConstDef                    {
        $$ = make_node("ConstDefList", -1, -1, $1);
    }
    | ConstDefList COMMA ConstDef {
        $$ = make_node("ConstDefList", -1, -1, $1, $2, $3);
    }
    ;

/* ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal */
ConstDef
    : IDENT ConstDimList ASSIGN ConstInitVal { 
        $$ = make_node("ConstDef", -1, -1, $1, $2, $3, $4);
    }
    | IDENT ASSIGN ConstInitVal { 
        $$ = make_node("ConstDef", -1, -1, $1, $2, $3);
    }
    ;

ConstDimList
    : LBRACK ConstExp RBRACK {
        $$ = make_node("ConstDimList", -1, -1, $1, $2, $3);
    }
    | ConstDimList LBRACK ConstExp RBRACK {
        $$ = make_node("ConstDimList", -1, -1, $1, $2, $3, $4);
    }
    ;

/* ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}' */
ConstInitVal
    : ConstExp {
        $$ = make_node("ConstInitVal", -1, -1, $1);
    }
    | LBRACE RBRACE {
        $$ = make_node("ConstInitVal", -1, -1, $1, $2);
    }
    | LBRACE ConstInitValList RBRACE {
        $$ = make_node("ConstInitVal", -1, -1, $1, $2, $3);
    }
    ;

ConstInitValList
    : ConstInitVal {
        $$ = make_node("ConstInitValList", -1, -1, $1);
    }
    | ConstInitValList COMMA ConstInitVal {
        $$ = make_node("ConstInitValList", -1, -1, $1, $2, $3);
    }
    ;

/* 基本类型 BType → 'int' | 'float' */
BType
    : INT {
        $$ = make_node("BType", -1, -1, $1);
    }
    | FLOAT {
        $$ = make_node("BType", -1, -1, $1);
    }
    ;

/* 变量声明 VarDecl → BType VarDef { ',' VarDef } ';' */
VarDecl
    : BType VarDefList SEMICOLON {
        $$ = make_node("VarDecl", -1, -1, $1, $2, $3);
    }
    ;

VarDefList
    : VarDef {
        $$ = make_node("VarDefList", -1, -1, $1);
    }
    | VarDefList COMMA VarDef {
        $$ = make_node("VarDefList", -1, -1, $1, $2, $3);
    }
    ;

/* VarDef → Ident { '[' ConstExp ']' }
 *        | Ident { '[' ConstExp ']' } '=' InitVal
 */
VarDef
    : IDENT VarDimList {
        $$ = make_node("VarDef", -1, -1, $1, $2);
    }
    | IDENT VarDimList ASSIGN InitVal {
        $$ = make_node("VarDef", -1, -1, $1, $2, $3, $4);
    }
    | IDENT {
        $$ = make_node("VarDef", -1, -1, $1);
    }
    | IDENT ASSIGN InitVal {
        $$ = make_node("VarDef", -1, -1, $1, $2, $3);
    }
    ;

VarDimList
    : LBRACK ConstExp RBRACK {
        $$ = make_node("VarDimList", -1, -1, $1, $2, $3);
    }
    | VarDimList LBRACK ConstExp RBRACK {
        $$ = make_node("VarDimList", -1, -1, $1, $2, $3, $4);
    }
    ;

/* InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}' */
InitVal
    : Exp {
        $$ = make_node("InitVal", -1, -1, $1);
    }
    | LBRACE RBRACE {
        $$ = make_node("InitVal", -1, -1, $1, $2);
    }
    | LBRACE InitValList RBRACE {
        $$ = make_node("InitVal", -1, -1, $1, $2, $3);
    }
    ;

InitValList
    : InitVal {
        $$ = make_node("InitValList", -1, -1, $1);
    }
    | InitValList COMMA InitVal {
        $$ = make_node("InitValList", -1, -1, $1, $2, $3);
    }
    ;

/* 函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block */
FuncDef
    : FuncType IDENT LPARENT FuncFParams RPARENT Block { 
        $$ = make_node("FuncDef", -1, -1, $1, $2, $3, $4, $5, $6);
    }
    | FuncType IDENT LPARENT RPARENT Block {
        $$ = make_node("FuncDef", -1, -1, $1, $2, $3, $4, $5);
    }
    ;

FuncType
    : VOID {
        $$ = make_node("FuncType", -1, -1, $1);
    }
    | INT {
        $$ = make_node("FuncType", -1, -1, $1);
    }
    | FLOAT {
        $$ = make_node("FuncType", -1, -1, $1);
    }
    ;

/* FuncFParams → FuncFParam { ',' FuncFParam } */
FuncFParams
    : FuncFParam {
        $$ = make_node("FuncFParams", -1, -1, $1);
    }
    | FuncFParams COMMA FuncFParam {
        $$ = make_node("FuncFParams", -1, -1, $1, $2, $3);
    }
    ;

/* FuncFParam → BType Ident ['[' ']' { '[' Exp ']' }] */
FuncFParam
    : BType IDENT {
        $$ = make_node("FuncFParam", -1, -1, $1, $2);
    }
    | BType IDENT LBRACK RBRACK {
        $$ = make_node("FuncFParam", -1, -1, $1, $2, $3, $4);
    }
    | BType IDENT LBRACK RBRACK FuncFParamDimsList {
        $$ = make_node("FuncFParam", -1, -1, $1, $2, $3, $4, $5);
    }
    ;

FuncFParamDimsList
    : LBRACK Exp RBRACK {
        $$ = make_node("FuncFParamDimsList", -1, -1, $1, $2, $3);
    }
    | FuncFParamDimsList LBRACK Exp RBRACK {
        $$ = make_node("FuncFParamDimsList", -1, -1, $1, $2, $3, $4);
    }
    ;

/* Block → '{' { BlockItem } '}' */
Block
    : LBRACE RBRACE {
        $$ = make_node("Block", -1, -1, $1, $2);
    }
    | LBRACE BlockItemList RBRACE {
        $$ = make_node("Block", -1, -1, $1, $2, $3);
    }
    | LBRACE error RBRACE // 恢复错误
    ;

BlockItemList
    : BlockItem {
        $$ = make_node("BlockItemList", -1, -1, $1);
    }
    | BlockItemList BlockItem {
        $$ = make_node("BlockItemList", -1, -1, $1, $2);
    }
    | BlockItemList error  /* 跳过坏语句 */
    ;

/* BlockItem → Decl | Stmt */
BlockItem
    : Decl {
        $$ = make_node("BlockItem", -1, -1, $1);
    }
    | Stmt {
        $$ = make_node("BlockItem", -1, -1, $1);
    }
    ;

/* Stmt 各种形式 */
Stmt:
    /* 赋值语句：LVal '=' Exp ';' */
    LVal ASSIGN Exp SEMICOLON {
        $$ = make_node("Stmt", -1, -1, $1, $2, $3, $4);
    }

    /* 表达式语句：[Exp] ';' */
    | SEMICOLON {
        $$ = make_node("Stmt", -1, -1, $1);
    }
    | Exp SEMICOLON {
        $$ = make_node("Stmt", -1, -1, $1, $2);
    }

    /* 语句块 */
    | Block {
        $$ = make_node("Stmt", -1, -1, $1);
    }

    /* if (Cond) Stmt [else Stmt] */
    | IF LPARENT Cond RPARENT Stmt %prec LOWER_THAN_ELSE {
        $$ = make_node("Stmt", -1, -1, $1, $2, $3, $4, $5);
    }
    | IF LPARENT Cond RPARENT Stmt ELSE Stmt {
        $$ = make_node("Stmt", -1, -1, $1, $2, $3, $4, $5, $6, $7);
    }

    /* while (Cond) Stmt */
    | WHILE LPARENT Cond RPARENT Stmt {
        $$ = make_node("Stmt", -1, -1, $1, $2, $3, $4, $5);
    }

    /* break; */
    | BREAK SEMICOLON {
        $$ = make_node("Stmt", -1, -1, $1, $2);
    }

    /* continue; */
    | CONTINUE SEMICOLON {
        $$ = make_node("Stmt", -1, -1, $1, $2);
    }

    /* return [Exp] ';' */
    | RETURN SEMICOLON {
        $$ = make_node("Stmt", -1, -1, $1, $2);
    }
    | RETURN Exp SEMICOLON {
        $$ = make_node("Stmt", -1, -1, $1, $2, $3);
    }

    /* 恢复错误 */
    | error SEMICOLON {
        $$ = 0; /* 该节点不加入语法树 */
    }
    ;

/* 表达式 Exp → AddExp */
Exp
    : AddExp {
        $$ = make_node("Exp", -1, -1, $1);
    }
    ;

/* 条件表达式 Cond → LOrExp */
Cond
    : LOrExp {
        $$ = make_node("Cond", -1, -1, $1);
    }
    ;

/* LVal → Ident {'[' Exp ']'} */
LVal
    : IDENT LValDimsList {
        $$ = make_node("LVal", -1, -1, $1, $2);
    }
    | IDENT {
        $$ = make_node("LVal", -1, -1, $1);
    }
    ;

LValDimsList
    : LBRACK Exp RBRACK {
        $$ = make_node("LValDimsList", -1, -1, $1, $2, $3);
    }
    | LValDimsList LBRACK Exp RBRACK {
        $$ = make_node("LValDimsList", -1, -1, $1, $2, $3, $4);
    }
    ;

/* PrimaryExp → '(' Exp ')' | LVal | Number */
PrimaryExp
    : LPARENT Exp RPARENT {
        $$ = make_node("PrimaryExp", -1, -1, $1, $2, $3);
    }
    | LVal {
        $$ = make_node("PrimaryExp", -1, -1, $1);
    }
    | Number {
        $$ = make_node("PrimaryExp", -1, -1, $1);
    }
    ;

/* Number → IntConst | FloatConst */
Number
    : INT_CONST {
        $$ = make_node("Number", -1, -1, $1);
    }
    | FLOAT_CONST {
        $$ = make_node("Number", -1, -1, $1);
    }
    ;

/* UnaryExp → PrimaryExp
 *          | Ident '(' [FuncRParams] ')'
 *          | UnaryOp UnaryExp
 */
UnaryExp
    : PrimaryExp {
        $$ = make_node("UnaryExp", -1, -1, $1);
    }
    | IDENT LPARENT RPARENT {
        $$ = make_node("UnaryExp", -1, -1, $1, $2, $3);
    }
    | IDENT LPARENT FuncRParams RPARENT {
        $$ = make_node("UnaryExp", -1, -1, $1, $2, $3, $4);
    }
    | UnaryOp UnaryExp {
        $$ = make_node("UnaryExp", -1, -1, $1, $2);
    }
    ;

/* UnaryOp → '+' | '−' | '!' */
UnaryOp
    : PLUS {
        $$ = make_node("UnaryOp", -1, -1, $1);
    }
    | MINUS {
        $$ = make_node("UnaryOp", -1, -1, $1);
    }
    | NOT {
        $$ = make_node("UnaryOp", -1, -1, $1);
    }
    ;

FuncRParams
    : Exp {
        $$ = make_node("FuncRParams", -1, -1, $1);
    }
    | FuncRParams COMMA Exp {
        $$ = make_node("FuncRParams", -1, -1, $1, $2, $3);
    }
    ;

/* MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp */
MulExp
    : UnaryExp {
        $$ = make_node("MulExp", -1, -1, $1);
    }
    | MulExp MUL UnaryExp {
        $$ = make_node("MulExp", -1, -1, $1, $2, $3);
    }
    | MulExp DIV UnaryExp {
        $$ = make_node("MulExp", -1, -1, $1, $2, $3);
    }
    | MulExp MOD UnaryExp {
        $$ = make_node("MulExp", -1, -1, $1, $2, $3);
    }
    ;

/* AddExp → MulExp | AddExp ('+' | '−') MulExp */
AddExp
    : MulExp {
        $$ = make_node("AddExp", -1, -1, $1);
    }
    | AddExp PLUS MulExp {
        $$ = make_node("AddExp", -1, -1, $1, $2, $3);
    }
    | AddExp MINUS MulExp {
        $$ = make_node("AddExp", -1, -1, $1, $2, $3);
    }
    ;

/* RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp */
RelExp
    : AddExp {
        $$ = make_node("RelExp", -1, -1, $1);
    }
    | RelExp LT AddExp {
        $$ = make_node("RelExp", -1, -1, $1, $2, $3);
    }
    | RelExp GT AddExp {
        $$ = make_node("RelExp", -1, -1, $1, $2, $3);
    }
    | RelExp LE AddExp {
        $$ = make_node("RelExp", -1, -1, $1, $2, $3);
    }
    | RelExp GE AddExp {
        $$ = make_node("RelExp", -1, -1, $1, $2, $3);
    }
    ;

/* EqExp → RelExp | EqExp ('==' | '!=') RelExp */
EqExp
    : RelExp {
        $$ = make_node("EqExp", -1, -1, $1);
    }
    | EqExp EQ RelExp {
        $$ = make_node("EqExp", -1, -1, $1, $2, $3);
    }
    | EqExp NEQ RelExp {
        $$ = make_node("EqExp", -1, -1, $1, $2, $3);
    }
    ;

/* LAndExp → EqExp | LAndExp '&&' EqExp */
LAndExp
    : EqExp {
        $$ = make_node("LAndExp", -1, -1, $1);
    }
    | LAndExp AND EqExp {
        $$ = make_node("LAndExp", -1, -1, $1, $2, $3);
    }
    ;

/* LOrExp → LAndExp | LOrExp '||' LAndExp */
LOrExp
    : LAndExp {
        $$ = make_node("LOrExp", -1, -1, $1);
    }
    | LOrExp OR LAndExp {
        $$ = make_node("LOrExp", -1, -1, $1, $2, $3);
    }
    ;

/* 常量表达式 ConstExp → AddExp */
ConstExp
    : AddExp {
        $$ = make_node("ConstExp", -1, -1, $1);
    }
    ;

%%  /* ================== 代码区 ================== */

void yyerror(const char *msg) {
    bison_error_handler();
}

int rank_token(yysymbol_kind_t token) {
    // TODO: 添加更多规则来排序输出的expected信息
    switch (token) {
        case YYSYMBOL_INT_CONST: return 1;
        case YYSYMBOL_FLOAT_CONST: return 2;
        default: return int(token);
    }
    return int(token);
}

bool cmp(yysymbol_kind_t a, yysymbol_kind_t b) {
    return rank_token(a) < rank_token(b);
}

static int
yyreport_syntax_error(const yypcontext_t *ctx)
{
    /* 1. 初始化错误信息 */
    string msg = "Syntax error";

    /* 2. 获取真正的“unexpected token” */
    yysymbol_kind_t unexpected = yypcontext_token(ctx);

    /* 3. 打印 unexpected token（使用你自己的规则） */
    if (unexpected != YYSYMBOL_YYEMPTY) {
        const char *name = yysymbol_name(unexpected);  /* Bison 内部名字，例如 "+" 或 ";" */
        if (strlen(name) == 1)
            msg += ", unexpected \"" + string(name) + "\"";
        else
            msg += ", unexpected <" + string(name) + ">";
    }

    /* 4. 获取 expected token 列表（最多展示 5 个） */
    yysymbol_kind_t expected[8];
    int n = yypcontext_expected_tokens(ctx, expected, 8);
    sort(expected, expected+n, cmp);

    if (n > 0) {
        msg += ", expecting ";
        for (int i = 0; i < min(2, n); i++) {
            if (i > 0) 
                msg += " or ";

            /* 打印 expected token，自定义规则 */
            msg += strlen(yysymbol_name(expected[i])) == 1 ? "\"" : "<";
            msg += yysymbol_name(expected[i]);
            msg += strlen(yysymbol_name(expected[i])) == 1 ? "\"" : ">";
        }
    }
    msg += '.';

    bison_error_handler(msg.c_str());
    return 0;   /* 0 = 告诉 Bison “继续正常的错误恢复行为” */
}
%option noyywrap nodefault yylineno

%{
#include "flex_config.h"
#include <stdlib.h>
#include <string.h>

// 自己版的yylval
// TODO： 使用Bison生成的TokenValue结构体
TokenValue yylval;

const char *tokenName(int token) {
    switch (token) {
        case CONST: return "CONST";
        case INT: return "INT";
        case FLOAT: return "FLOAT";
        case VOID: return "VOID";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case BREAK: return "BREAK";
        case CONTINUE: return "CONTINUE";
        case RETURN: return "RETURN";
        case EQ: return "EQ";
        case NEQ: return "NEQ";
        case LE: return "LE";
        case GE: return "GE";
        case AND: return "AND";
        case OR: return "OR";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case MOD: return "MOD";
        case LT: return "LT";
        case GT: return "GT";
        case ASSIGN: return "ASSIGN";
        case NOT: return "NOT";
        case SEMICOLON: return "SEMICOLON";
        case COMMA: return "COMMA";
        case LPARENT: return "LPARENT";
        case RPARENT: return "RPARENT";
        case LBRACK: return "LBRACK";
        case RBRACK: return "RBRACK";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case BAD_OCT: return "BAD_OCT";
        case BAD_HEX: return "BAD_HEX";
        case BAD_FLOAT: return "BAD_FLOAT";
        case FLOAT_CONST: return "FLOAT_CONST";
        case INT_CONST: return "INT_CONST";
        case IDENT: return "IDENT";
        case BAD_IDENT: return "BAD_IDENT";
        default: return "UNKNOWN_CHAR";
    }
}
%}

/* ---------- 定义部分 ---------- */
/* 标识符：首字符是字母或下划线，后面字母/数字/下划线 */
LETTER      [A-Za-z_]
DIGIT       [0-9]
IDENT       {LETTER}({LETTER}|{DIGIT})*
BAD_IDENT   {DIGIT}({LETTER}|{DIGIT})*
/* 带错误发现的数字匹配 */
/* 十六进制 */
HEX_PREFIX      0[xX]
HEX_DIG         [0-9a-fA-F]

/* 八进制合法部分 */
OCT_DIG         [0-7]

/* 八进制错误：如 0, 00, 012 后面出现 8 或 9 */
BAD_OCT       0[0-7]*[89][0-9]*

/* 十六进制错误：如 0x1G, 0X2Z */
BAD_HEX     {HEX_PREFIX}(({HEX_DIG}*[g-zG-Z][0-9a-zA-Z]*)*)

/* 十进制 */
DECIMAL         [1-9][0-9]*

/* 正常整数构造 */
INTCONST        ({HEX_PREFIX}{HEX_DIG}+|0|0{OCT_DIG}+|{DECIMAL})

/* 浮点 */
/* 错误浮点：多个小数点，例如 3.14.15 */
BAD_FLOAT       [0-9]+(\.[0-9]+){2,}

/* 合法浮点构造 */
F1              [0-9]+\.[0-9]*([eE][+-]?[0-9]+)?
F2              \.[0-9]+([eE][+-]?[0-9]+)?
F3              [0-9]+[eE][+-]?[0-9]+

FLOATCONST      ({F1}|{F2}|{F3})

/* 空白符：空格、制表、换行等 */
WS          [ \t\r\n]+

/* 多行注释状态 */
%x COMMENT

/* ---------- 规则部分 ---------- */
%%
 /* 关键字 */
"const"         { return CONST; }
"int"           { return INT; }
"float"         { return FLOAT; }
"void"          { return VOID; }

"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }
"return"        { return RETURN; }

 /* 运算符和分隔符 */
"=="            { return EQ; }
"!="            { return NEQ; }
"<="            { return LE; }
">="            { return GE; }
"&&"            { return AND; }
"||"            { return OR; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MUL; }
"/"             { return DIV; }
"%"             { return MOD; }

"<"             { return LT; }
">"             { return GT; }
"="             { return ASSIGN; }
"!"             { return NOT; }

";"             { return SEMICOLON; }
","             { return COMMA; }

"("             { return LPARENT; }
")"             { return RPARENT; }
"["             { return LBRACK; }
"]"             { return RBRACK; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }

 /* ---------- 数字错误处理 ---------- */
 /* 非法八进制：例如 0128, 0779, 00089 */
{BAD_OCT} {
    return BAD_OCT;
}

{BAD_HEX} {
    return BAD_HEX;
}

 /* 多个小数点：如 3.14.15 */
{BAD_FLOAT} {
    return BAD_FLOAT;
}

 /* ---------- 正确数字 ---------- */
  /* 整数：十进制 / 八进制 / 十六进制 */
{INTCONST}    {
    yylval.ival = (int)strtol(yytext, NULL, 0);
    return INT_CONST;
}

 /* 浮点 */
{FLOATCONST}  {
    yylval.fval = strtof(yytext, NULL);
    return FLOAT_CONST;
}

 /* 错误的标识符：如 123abc, 0x1A */
{BAD_IDENT}   {
    return BAD_IDENT;
}

 /* 标识符 */
{IDENT}         {
                    /* 改成你 parser.y 里的字段名，比如 ident / id / str 等 */
                    yylval.ident = strdup(yytext);
                    return IDENT;
                }

 /* 空白直接丢掉 */
{WS}            { /* ignore whitespace */ }

 /* 单行注释：// 到行尾 */
"//".*          { /* ignore single-line comment */ }

 /* 多行注释：用独立状态处理，支持跨行 */
"/*"            { BEGIN(COMMENT); }

<COMMENT>"*/"   { BEGIN(INITIAL); }
<COMMENT>\n     { /* yylineno 已经会自动 +1，一般不用管 */ }
<COMMENT>.      { /* 丢弃注释里其他字符 */ }

 /* 文件结束时，如果还在注释状态，可以在这里报个错（可选） */
<COMMENT><<EOF>> {
                    /* 可以选择返回一个错误 token，或者直接结束 */
                    /* 这里只是结束扫描 */
                    return 0;
                }

 /* 任何没匹配到的字符，简单报错（或者直接丢弃，看你需求） */
.               {
                    return UNKNOWN_CHAR;
                }

%%

/* 你也可以在这里定义 yywrap/辅助函数，但用了 %option noyywrap 就不用再写 yywrap 了 */

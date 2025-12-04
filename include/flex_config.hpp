#ifndef FLEX_CONFIG_HPP
#define FLEX_CONFIG_HPP

// #include <FlexLexer.h> // 已经在 generated/lexer.cpp 中包含过了，重新包含会报错
#include <cstdio>

// 定义词法分析器返回的token类型
enum yytoken {
    CONST = 256,
    INT,
    FLOAT,
    VOID,
    IF,
    ELSE,
    WHILE,
    BREAK,
    CONTINUE,
    RETURN,
    EQ,
    NEQ,
    LE,
    GE,
    AND,
    OR,
    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,
    LT,
    GT,
    ASSIGN,
    NOT,
    SEMICOLON,
    COMMA,
    LPARENT,
    RPARENT,
    LBRACK,
    RBRACK,
    LBRACE,
    RBRACE,
    FLOAT_CONST,
    INT_CONST,
    IDENT,
    BAD_OCT,
    BAD_HEX,
    BAD_FLOAT,
    BAD_IDENT
};

const char *tokenName(int token);

// 定义yylval的结构体
struct TokenValue {
    // 整数常量
    int ival;
    // 浮点常量
    double fval;
    // 字符串/标识符名
    char *ident = nullptr;
};

extern TokenValue yylval; // 自己版的“yylval”

#endif
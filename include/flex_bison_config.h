#ifndef FLEX_BISON_CONFIG_HPP
#define FLEX_BISON_CONFIG_HPP

#include <stdio.h>
#include "bison.h"

const char *tokenName(int token);

// Flex部分
extern FILE *yyin;
extern int yylineno;
extern char *yytext;
extern int yylex(void);

// Bison部分
extern int yychar;
extern void yyerror(const char *msg);

#endif
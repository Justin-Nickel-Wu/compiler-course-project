#include "flex_bison_config.h"
#include "error_handler.hpp"
#include <bits/stdc++.h>
using namespace std;

void Err(char type, int line, string desc) {
    cout << red("Error type " + string(1, type) + " at Line " + to_string(line));
    if (!desc.empty()) {
        cout << red(": " + desc) << endl;
    } else {
        cout << red(".") << endl;
    }
}

void Ok(string desc) {
    cout << green("OK: " + desc) << endl;
}

bool ONLY_CHECK_LEXER = false;
bool WRONG_FOUND_IN_LEXER = false;

void flex_error_handler(int token) {
    switch (token) {
        case BAD_OCT: {
            Err('A', yylineno, "Invalid octal constant: " + string(yytext));
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        case BAD_HEX: {
            Err('A', yylineno, "Invalid hex constant: " + string(yytext));
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        case BAD_FLOAT: {
            Err('A', yylineno, "Invalid float constant: " + string(yytext));
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        case BAD_IDENT: {
            Err('A', yylineno, "Invalid identifier: " + string(yytext));
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        case UNKNOWN_CHAR: {
            Err('A', yylineno, "Unknown character: " + string(yytext));
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        // 出现意外情况
        default: {
            cout << red("Unknown lexer error at Line " + to_string(yylineno) + ".") << endl;
            exit(1);
        }
    }

    if (!ONLY_CHECK_LEXER) { // 仅词法分析时不退出
        cout << red("\nTerminating due to lexer error.") << endl;
        exit(1);
    }
}

void bison_error_handler(const char *msg) {
    Err('B', yylineno, msg);
    // cout << red("\nTerminating due to parser error.") << endl;
    // exit(1);
}
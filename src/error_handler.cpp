#include "flex_bison_config.hpp"
#include "error_handler.hpp"
#include <bits/stdc++.h>
using namespace std;

bool WRONG_FOUND_IN_LEXER = false;
bool WRONG_FOUND_IN_PARSER = false;

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

void flex_error_handler(int token) {
    WRONG_FOUND_IN_LEXER = true;
    switch (token) {
        case BAD_OCT: {
            Err('A', yylineno, "Invalid octal constant: \"" + string(yytext) + "\".");
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        case BAD_HEX: {
            Err('A', yylineno, "Invalid hex constant: \"" + string(yytext) + "\".");
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        case BAD_FLOAT: {
            Err('A', yylineno, "Invalid float constant: \"" + string(yytext) + "\".");
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        case BAD_IDENT: {
            Err('A', yylineno, "Invalid identifier: \"" + string(yytext) + "\".");
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        case UNKNOWN_CHAR: {
            Err('A', yylineno, "Unknown character: \"" + string(yytext) + "\".");
            WRONG_FOUND_IN_LEXER = true;
            break;
        }
        // 出现意外情况
        default: {
            cout << red("Unknown lexer error at Line " + to_string(yylineno) + ".") << endl;
            exit(1);
        }
    }
}

void bison_error_handler(const char *msg) {
    WRONG_FOUND_IN_PARSER = true;
    Err('B', yylineno, msg);
    // cout << red("\nTerminating due to parser error.") << endl;
    // exit(1);
}
#include <bits/stdc++.h>
#include "flex_bison_config.h"
#include "error_handler.hpp"
using namespace std;

void part1_test() {
    struct info {
        int token, line;
        string text;
        YYSTYPE val;
        info(int t, int l, string txt, YYSTYPE v) : token(t), line(l), text(txt), val(v) {}
    };

    FILE *file = fopen("input.txt", "r");
    if (!file) {
        cout << red("Failed to open input.txt\n") << endl;
        return;
    }
    cout << green("File opened successfully.\n") << endl;

    int token;
    vector<info> tokens;

    yyin = file;
    // ONLY_CHECK_LEXER = true;
    while ((token = yylex()) != 0)
        tokens.push_back(info(token, yylineno, yytext, yylval));

    if (!WRONG_FOUND_IN_LEXER) {
        Ok("No errors found.");
        for (int i = 0; i < tokens.size(); i++) {
            if (i == 0 || tokens[i - 1].line != tokens[i].line)
                cout << "Line " << tokens[i].line << ":" << endl;
            cout << "   ";
            cout << tokens[i].text << " -> " << tokenName(tokens[i].token) << endl;
        }
    }
}

int main() {
    part1_test();
    return 0;
}
#include <bits/stdc++.h>
#include "flex_bison_config.hpp"
#include "error_handler.hpp"
using namespace std;

void set_input_file(string filename) {
    FILE *file = fopen(filename.c_str(), "r");
    if (!file) {
        cout << red("Failed to open " + filename + "\n") << endl;
        exit(1);
    }
    cout << green("File opened successfully.\n") << endl;
    yyin = file;
}

void part1_test(string filename) {
    set_input_file(filename);

    struct info {
        int token, line;
        string text;
        YYSTYPE val;
        info(int t, int l, string txt, YYSTYPE v) : token(t), line(l), text(txt), val(v) {}
    };
    vector<info> tokens;
    int token;

    // ONLY_CHECK_LEXER = true;
    while ((token = yylex()) != 0)
        tokens.push_back(info(token, yylineno, yytext, yylval));

    if (!WRONG_FOUND_IN_LEXER) {
        Ok("No lex errors found.");
        for (int i = 0; i < tokens.size(); i++) {
            if (i == 0 || tokens[i - 1].line != tokens[i].line)
                cout << "Line " << tokens[i].line << ":" << endl;
            cout << "   ";
            cout << tokens[i].text << " -> " << tokenName(tokens[i].token) << endl;
        }
    }
}

void part2_test(string filename) {
    set_input_file(filename);
    yyparse();
    if (WRONG_FOUND_IN_LEXER || WRONG_FOUND_IN_PARSER)
        cout << red("\nParsing completed with errors.") << endl;
    else
        Ok("No syntax errors found.");
}

int main() {
    // part1_test("input.txt");
    cout << endl
         << endl;
    // yydebug = 1;
    part2_test("input.txt");
    return 0;
}
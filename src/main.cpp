#include <bits/stdc++.h>
#include <FlexLexer.h> // 必须要包含
#include "flex_config.hpp"
using namespace std;

template <typename T>
string red(const T &input) {
    ostringstream oss;
    oss << "\033[31m" << input << "\033[0m";
    return oss.str();
}

template <typename T>
string green(const T &input) {
    std::ostringstream oss;
    oss << "\033[32m" << input << "\033[0m";
    return oss.str();
}

void Err(char type, int line, string desc = "") {
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

void part1_test() {
    struct info {
        int token, line;
        string text;
        TokenValue val;
        info(int t, int l, string txt, TokenValue v) : token(t), line(l), text(txt), val(v) {}
    };

    ifstream fin("input.txt");
    if (!fin.is_open()) {
        cout << red("Failed to open input.txt\n") << endl;
        return;
    }
    cout << green("File opened successfully.") << endl;

    yyFlexLexer lexer;
    int token;
    bool something_wrong = false;
    vector<info> tokens;

    lexer.switch_streams(&fin, nullptr);
    while ((token = lexer.yylex()) != 0) {
        switch (token) {
            case BAD_OCT: {
                Err('A', lexer.lineno(), "Invalid octal constant: " + string(lexer.YYText()));
                something_wrong = true;
                break;
            }
            case BAD_HEX: {
                Err('A', lexer.lineno(), "Invalid hex constant: " + string(lexer.YYText()));
                something_wrong = true;
                break;
            }
            case BAD_FLOAT: {
                Err('A', lexer.lineno(), "Invalid float constant: " + string(lexer.YYText()));
                something_wrong = true;
                break;
            }
            case BAD_IDENT: {
                Err('A', lexer.lineno(), "Invalid identifier: " + string(lexer.YYText()));
                something_wrong = true;
                break;
            }
            default: {
                tokens.push_back(info(token, lexer.lineno(), lexer.YYText(), yylval));
            }
        }
    }
    fin.close();
    if (!something_wrong) {
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
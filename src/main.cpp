#include <bits/stdc++.h>
#include <FlexLexer.h> // 必须要包含
#include "flex_config.hpp"
using namespace std;

int main() {
    printf("main sees yylval at %p\n", (void *)&yylval);
    yyFlexLexer lexer;
    ifstream fin("input.txt");
    if (!fin.is_open()) {
        cerr << "Failed to open input.txt" << endl;
        return 1;
    }
    lexer.switch_streams(&fin, nullptr);

    int tok;
    while ((tok = lexer.yylex()) != 0) {
        cout << "Line " << lexer.lineno() << " " << lexer.YYText() << " -> " << tokenName(tok) << endl;
        cout << "   " << "fval: " << yylval.fval << ", ival: " << yylval.ival << ", ident: " << (yylval.ident == NULL ? "NULL" : yylval.ident) << endl;
    }

    return 0;
}
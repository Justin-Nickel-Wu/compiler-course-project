#include <bits/stdc++.h>
#include "flex_bison_config.hpp"
#include "error_handler.hpp"
#include "parse_tree.hpp"
#include "SemanticAnalyzer.hpp"
using namespace std;

void set_input_file(string filename) {
    FILE *file = fopen(filename.c_str(), "r");
    if (!file) {
        cout << red("Failed to open " + filename + "\n") << endl;
        exit(1);
    }
    Ok("File opened successfully.\n");
    yyin = file;
}

void part1_test(string filename) {
    set_input_file(filename);

    struct info {
        int     token, line;
        string  text;
        YYSTYPE val;
        info(int t, int l, string txt, YYSTYPE v) : token(t), line(l), text(txt), val(v) {}
    };
    vector<info> tokens;
    int          token;

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
    cout << "root: " << GLOBAL_PARSE_TREE.get_root() << endl;
    GLOBAL_PARSE_TREE.debug(GLOBAL_PARSE_TREE.get_root());
}

void Process(const string &filename) {
    set_input_file(filename);
    yydebug         = 0; // 控制bison自带debug输出
    debug_output_id = 1; // 控制是否输出语法树时包含节点编号

    yyparse();
    if (WRONG_FOUND_IN_LEXER || WRONG_FOUND_IN_PARSER) {
        cout << '\n';
        cout << red("Parsing failed due to earlier errors.") << endl;
        return;
    } else {
        Ok("Parsing completed successfully.\n");
        Ok("Generating parse tree to output/parse_tree.png\n");
    }
    to_dot("parse_tree");
    SemanticAnalyzer analyzer(GLOBAL_PARSE_TREE);
    analyzer.OUTPUT_VAR_LIST = true; // DEBUG输出变量列表
    if (analyzer.SemanticAnalyze()) {
        Ok("Semantic analysis completed successfully.\n");
    } else {
        cout << red("\nSemantic analysis failed due to earlier errors.") << endl;
    }

    analyzer.printASTInfo(); // DEBUG输出AST辅助信息
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Usage: make run FILE=your_input_file" << endl;
        return 1;
    }
    Process(argv[1]);
    return 0;
}
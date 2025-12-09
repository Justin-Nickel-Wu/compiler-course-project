#ifndef SemanticAnalyzer_hpp
#define SemanticAnalyzer_hpp

#include <bits/stdc++.h>
using namespace std;

#include "parse_tree.hpp"

struct SymbolInfo {
    // 类型，使用词法token
    int type;
    // TODO:是否为数组
    // bool is_array;

    SymbolInfo() : type(-1) {}
    SymbolInfo(int type) : type(type) {}
};

class SemanticAnalyzer {
private:
    ParseTree &AST;
    // 语义分析过程中是否出错
    bool SOMETHING_WRONG;
    // 当前处理的变量类型
    int GLOBAL_VAR_TYPE;
    // 符号表栈
    vector<unordered_map<string, SymbolInfo>> scope_stack;

    // 创建作用域
    void push_scope();
    // 销毁作用域
    void pop_scope();
    // 声明符号，如果成功返回true，失败返回false（重复定义）
    bool declare_symbol(int type, const string &ident);
    // 语义分析递归函数
    void SemanticAnalyzeDFS(int p);

public:
    SemanticAnalyzer(ParseTree &ast) : AST(ast) {}
    // 进行语义分析
    bool SemanticAnalyze();
};

#endif
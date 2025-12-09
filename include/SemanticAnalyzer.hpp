#ifndef SemanticAnalyzer_hpp
#define SemanticAnalyzer_hpp

#include <bits/stdc++.h>
using namespace std;

#include "parse_tree.hpp"

enum SymbolType {
    VAR_SYMBOL,
    FUNC_SYMBOL
};

struct SymbolInfo {
    // 类型，使用词法token
    int type;

    SymbolType symbol_type;

    SymbolInfo() : type(-1), symbol_type(VAR_SYMBOL) {}
    SymbolInfo(int type, SymbolType symbol_type) : type(type), symbol_type(symbol_type) {}
};

class SemanticAnalyzer {
private:
    ParseTree &AST;
    // 语义分析过程中是否出错
    bool SOMETHING_WRONG;

    // 为了处理嵌套逻辑，使用类似mut方法
    int IN_LOOP;
    int IN_FUNC_DEF, IN_FUNC_IDENT_DEF, IN_FUNC_FPARAMS_DEF;

    // 当前处理的变量类型
    int GLOBAL_VAR_TYPE;
    // 函数形参栈
    vector<pair<int, string>> func_fparams_stack;
    // 符号表栈
    vector<unordered_map<string, SymbolInfo>> scope_stack;

    // 创建作用域
    void push_scope();
    // 销毁作用域
    void pop_scope();
    // 声明变量，失败返回false（重复定义）
    bool declare_var(int type, const string &ident);
    // 声明函数，失败返回false（重复定义）
    bool declare_func(int return_type, const string &ident);
    // 根据标识符查找信息
    SymbolInfo find(const string &ident);
    // 语义分析递归函数
    void SemanticAnalyzeDFS(int p);

public:
    SemanticAnalyzer(ParseTree &ast) : AST(ast) {}
    // 进行语义分析
    bool SemanticAnalyze();
};

#endif
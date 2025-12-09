#ifndef SemanticAnalyzer_hpp
#define SemanticAnalyzer_hpp

#include <bits/stdc++.h>
using namespace std;

#include "parse_tree.hpp"

struct SymbolInfo {
    // 类型 0:int 1:float 2:void
    int type;
    // TODO:是否为数组
    // bool is_array;

    SymbolInfo() : type(-1) {}
    SymbolInfo(int type) : type(type) {}
};

class SemanticAnalyzer {
private:
    // 符号表栈
    vector<unordered_map<string, SymbolInfo>> scope_stack;
    // 创建作用域
    void push_scope();
    // 销毁作用域
    void pop_scope();
    // 声明符号，如果成功返回true，失败返回false
    bool declare_symbol(const string &ident, int type);

public:
    bool SemanticAnalyze(const ParseTree &parse_tree);
};

#endif
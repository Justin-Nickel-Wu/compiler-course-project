#include "SemanticAnalyzer.hpp"
#include "parse_tree.hpp"

#include <bits/stdc++.h>
using namespace std;

void SemanticAnalyzer::push_scope() {
    scope_stack.push_back(unordered_map<string, SymbolInfo>());
}

void SemanticAnalyzer::pop_scope() {
    if (scope_stack.empty()) {
        cerr << "Error: Attempt to pop from an empty scope stack." << endl;
        exit(1);
    }
    scope_stack.pop_back();
}

bool SemanticAnalyzer::declare_symbol(const string &ident, int type) {
    if (scope_stack.empty()) {
        cerr << "Error: No scope available to declare symbol." << endl;
        exit(1);
    }
    auto &current = scope_stack.back();

    // 如果变量重复定义
    if (current.find(ident) != current.end()) {
        return false;
    }

    SymbolInfo info(type);
    current[ident] = info;

    return true;
}

bool SemanticAnalyzer::SemanticAnalyze(const ParseTree &parse_tree) {
    // 语义分析的具体实现待补充
    // 这里只是一个框架示例

    // 初始化全局作用域
    push_scope();

    // 遍历语法树节点进行语义检查
    // 具体逻辑根据语法树结构和语义规则实现

    // 最后销毁全局作用域
    pop_scope();

    return true; // 返回是否通过语义分析
}
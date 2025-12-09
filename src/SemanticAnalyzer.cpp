#include "SemanticAnalyzer.hpp"
#include "parse_tree.hpp"
#include "error_handler.hpp"

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

bool SemanticAnalyzer::declare_symbol(int type, const string &ident) {
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

bool SemanticAnalyzer::SemanticAnalyze() {
    // 初始化全局变量与全局作用域。
    SOMETHING_WRONG = false;
    GLOBAL_VAR_TYPE = -1;
    scope_stack.clear();
    push_scope();

    SemanticAnalyzeDFS(AST.get_root());

    // 最后销毁全局作用域
    pop_scope();

    return !SOMETHING_WRONG;
}

void SemanticAnalyzer::SemanticAnalyzeDFS(int p) {
    ParseTreeNode &node = AST.nodes[p];

    // 进入新作用域
    if (node.name == "Block") {
        push_scope();
    }

    // 处理声明变量时的类型
    if (node.name == "BType") {
        int q = node.son[0]; // 获取类型节点
        AST.nodes[q].token_type = node.token_type;
    }

    // 处理变量声明时的标识符
    if (node.name == "VarDef") {
        int q = node.son[0]; // 获取标识符节点
        string ident = AST.nodes[q].ident;
        if (!declare_symbol(GLOBAL_VAR_TYPE, ident)) {
            SOMETHING_WRONG = true;
            Err('2', node.line, "Redefinition of variable \"" + ident + "\".");
        }
    }

    // TODO: 其他语义分析逻辑

    // 递归处理子节点
    for (int son_id : node.son) {
        SemanticAnalyzeDFS(son_id);
    }

    // 离开作用域
    if (node.name == "Block") {
        pop_scope();
    }
}
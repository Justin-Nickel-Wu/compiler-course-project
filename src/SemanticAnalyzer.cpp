#include "SemanticAnalyzer.hpp"
#include "parse_tree.hpp"
#include "error_handler.hpp"
#include "bison.hpp"

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

bool SemanticAnalyzer::declare_var(int type, const string &ident) {
    if (scope_stack.empty()) {
        cerr << "Error: No scope available to declare symbol." << endl;
        exit(1);
    }
    auto &current = scope_stack.back();

    // 如果变量重复定义
    if (current.find(ident) != current.end()) {
        return false;
    }

    SymbolInfo info(type, VAR_SYMBOL);
    current[ident] = info;

    return true;
}

SymbolInfo SemanticAnalyzer::find(const string &ident) {
    SymbolInfo ret;
    for (auto it : scope_stack) {
        if (it.find(ident) != it.end()) {
            return it[ident];
        }
    }
    return ret;
}

bool SemanticAnalyzer::declare_func(int return_type, const string &ident) {
    // 函数声明时一定处于全局作用域中
    auto &current = scope_stack.front();
    SymbolInfo info(return_type, FUNC_SYMBOL);
    if (current.find(ident) != current.end()) {
        return false;
    }
    current[ident] = info;
    return true;
}

bool SemanticAnalyzer::SemanticAnalyze() {
    // 初始化全局变量与全局作用域。
    SOMETHING_WRONG = false;
    IN_LOOP = false;
    IN_FUNC_DEF = false;
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

    /*-------------------状态处理-------------------*/
    // 进入新作用域
    if (node.name == "Block") {
        push_scope();
    }

    // 进入循环体
    if (node.name == "Stmt" && AST.nodes[node.son[0]].token_type == WHILE) {
        IN_LOOP = true;
    }

    // 进入函数定义
    if (node.name == "FuncType") {
        GLOBAL_VAR_TYPE = AST.nodes[node.son[0]].token_type;
        IN_FUNC_DEF = true;
    }

    /*-------------------语义提取-------------------*/
    // 处理声明变量时的类型
    if (node.name == "BType") {
        int q = node.son[0]; // 获取类型节点
        GLOBAL_VAR_TYPE = AST.nodes[q].token_type;
    }

    // 处理变量声明时的标识符
    if (node.name == "VarDef") {
        int q = node.son[0]; // 获取标识符节点
        string ident = AST.nodes[q].ident;
        if (!declare_var(GLOBAL_VAR_TYPE, ident)) {
            SOMETHING_WRONG = true;
            Err('2', node.line, "Redefinition of variable \"" + ident + "\".");
        }
    }

    // 处理变量使用
    if (node.name == "LVal") {
        string ident = AST.nodes[node.son[0]].ident;
        SymbolInfo info = find(ident);
        // 使用未声明的变量
        if (info.type == -1) {
            SOMETHING_WRONG = true;
            Err('1', node.line, "Use of undeclared variable \"" + ident + "\".");
        }
        // 混淆函数与变量
        if (info.symbol_type == FUNC_SYMBOL) {
            SOMETHING_WRONG = true;
            Err('6', node.line, "\"" + ident + "\" is a function, not a variable.");
        }
    }

    // 处理函数声明
    if (IN_FUNC_DEF && node.token_type == IDENT) {
        if (!declare_func(GLOBAL_VAR_TYPE, node.ident)) {
            SOMETHING_WRONG = true;
            Err('4', node.line, "Redefinition of function \"" + string(node.ident) + "\".");
        }
    }

    // 处理break和continue语句
    if (!IN_LOOP && (node.token_type == BREAK || node.token_type == CONTINUE)) {
        SOMETHING_WRONG = true;
        Err('3', node.line, "\"break\" or \"continue\" statement not within a loop.");
    }

    // TODO: 其他语义分析逻辑

    /*-------------------递归遍历-------------------*/
    // 递归处理子节点
    for (int son_id : node.son) {
        SemanticAnalyzeDFS(son_id);
    }

    /*-------------------状态回收-------------------*/
    // 离开作用域
    if (node.name == "Block") {
        pop_scope();
    }

    // 离开循环体
    if (node.name == "Stmt" && AST.nodes[node.son[0]].token_type == WHILE) {
        IN_LOOP = false;
    }

    // 离开函数定义
    if (IN_FUNC_DEF && node.token_type == LPARENT) {
        IN_FUNC_DEF = false;
    }
}
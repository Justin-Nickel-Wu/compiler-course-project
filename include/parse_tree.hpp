#ifndef PARSE_TREE_HPP
#define PARSE_TREE_HPP

#include <bits/stdc++.h>
using namespace std;

#include "bison.hpp"

struct ParseTreeNode {
    string name;
    vector<int> son;
    int line;       // 所在行号
    int token_type; // 词法单元类型，非终结符为-1
    int ival;
    double fval;
    char *ident;
    ParseTreeNode(const string &n, int line, int token_type) : name(n), line(line), token_type(token_type) {}
};

struct ParseTree {
    int root;
    vector<ParseTreeNode> nodes;

    // 初始化，从1开始标号，0号节点为null节点
    ParseTree() : root(-1) { nodes.push_back(ParseTreeNode("null", -1, -1)); }
    // 创建新节点，返回节点编号
    int make_node(const string &name, int line, int token_type, initializer_list<int> _son);
    // 设置根节点编号
    void set_root(int root_id) { root = root_id; }
    // 获取根节点编号
    int get_root() const { return root; }
    // DEBUG输出对应编号节点及其子树
    void debug(int node_id);
};

// 全局唯一的语法树对象
extern ParseTree GLOBAL_PARSE_TREE;

// 为全局语法树创建新节点。line若为-1则会自动填充。token_type若为-1则表示非终结符
template <typename... Args>
int make_node(const string &name, int line, int token_type, Args... args) {
    return GLOBAL_PARSE_TREE.make_node(name, line, token_type, {args...});
}

// 为全局语法树创建新节点,用于符号token
int make_leaf(const string &name, int line, int token_type);

// 为全局语法树创建新节点,用于整型常量
int make_int_leaf(const string &name, int line, int token_type, int ival);

// 为全局语法树创建新节点,用于浮点常量
int make_float_leaf(const string &name, int line, int token_type, double fval);

// 为全局语法树创建新节点,用于标识符
int make_ident_leaf(const string &name, int line, int token_type, const char *ident);

// 设置全局语法树的根节点
void set_root(int root_id);

#endif
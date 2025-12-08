#include <bits/stdc++.h>
using namespace std;

#include "parse_tree.hpp"

ParseTree GLOBAL_PARSE_TREE;

int ParseTree::make_node(const string &name, int line, int token_type, initializer_list<int> _son) {
    int ret = nodes.size();
    nodes.push_back(ParseTreeNode(name, line, token_type));
    for (auto j : _son)
        nodes[ret].son.push_back(j);

    // 非终结符行号参数会输入-1，使用右部第一个符号的行号作为当前单元的行号
    if (line == -1) {
        if (_son.size() == 0) { // 没有孩子报错，即不允许生成epsilon的语法
            cout << "fatal error: can't use epsilon grammer." << endl;
            exit(1);
        }
        nodes[ret].line = nodes[*_son.begin()].line;
    }
    return ret;
}

void ParseTree::debug(int node_id) {
    cout << "Node " << node_id << ": " << nodes[node_id].name << '(' << nodes[node_id].line << ')' << endl;
    for (auto son_id : nodes[node_id].son) {
        cout << "  |- Son Node " << son_id << ": " << (son_id != 0 ? nodes[son_id].name : "null");
        if (son_id != 0)
            cout << " (" << nodes[son_id].line << ")";
        cout << endl;
    }
    cout << endl;
    for (auto son_id : nodes[node_id].son) {
        if (son_id != 0 && nodes[son_id].token_type == -1) // 非终结符才继续往下遍历
            debug(son_id);
    }
}

void ParseTree::to_dot(const string &filename) {
    ofstream out("output/" + filename + ".dot");

    out << "digraph ParseTree {" << endl;

    out << "    graph [ordering=\"out\"];" << endl; // 保持节点插入顺序

    // 格式化输出所有节点
    for (int i = 1; i < nodes.size(); i++) {
        out << "    ";
        out << "node" << i << " [label=\"" << nodes[i].name << "\"";
        if (nodes[i].token_type != -1) {
            out << ", shape=box" << ", color=green";
        }
        out << "];" << endl;
    }

    // 输出连边信息
    queue<int> q;
    q.push(root);
    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        for (auto son : nodes[curr].son) {
            if (son == 0) {
                cout << "Error: null son node found in to_dot()." << endl;
                exit(1);
            }
            out << "    ";
            out << "node" << curr << " -> " << "node" << son << ';' << endl;
            q.push(son);
        }
    }

    out << '}' << endl;
    out.close();
    // 转为png图片
    system(("dot -Tpng output/" + filename + ".dot -o output/" + filename + ".png").c_str());
}

int make_leaf(const string &name, int line, int token_type) {
    int ret = GLOBAL_PARSE_TREE.make_node(name, line, token_type, {});
    return ret;
}

int make_int_leaf(const string &name, int line, int token_type, int ival) {
    int ret = GLOBAL_PARSE_TREE.make_node(name, line, token_type, {});
    GLOBAL_PARSE_TREE.nodes[ret].ival = ival;
    return ret;
}

int make_float_leaf(const string &name, int line, int token_type, double fval) {
    int ret = GLOBAL_PARSE_TREE.make_node(name, line, token_type, {});
    GLOBAL_PARSE_TREE.nodes[ret].fval = fval;
    return ret;
}

int make_ident_leaf(const string &name, int line, int token_type, const char *ident) {
    int ret = GLOBAL_PARSE_TREE.make_node(name, line, token_type, {});
    GLOBAL_PARSE_TREE.nodes[ret].ident = strdup(ident);
    return ret;
}

void set_root(int root_id) {
    GLOBAL_PARSE_TREE.set_root(root_id);
}

void to_dot(const string &filename) {
    GLOBAL_PARSE_TREE.to_dot(filename);
}
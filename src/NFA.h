#ifndef NFA_H

#define NFA_H
#include "GraphStruct.h"
#include <bits/stdc++.h>
using namespace std;

class NFA {
    
    friend class DFA;

private:
    string regexp;               // 原始正则表达式
    vector<char> splited_regexp; // 插入连接符后的正则表达式
    vector<char> postfix_regexp; // 后缀表达式
    GraphStruct nfa;

    bool need_split(char ch1, char ch2); // 判断是否需要在两个字符之间插入连接符
    int op_priority(char op);
    void create_symbol(stack<int> &sta, char ch); // 添加一个字符的NFA片段
    void union_(stack<int> &sta); // 处理｜操作符
    void concat(stack<int> &sta); // 处理.操作符
    void closure(stack<int> &sta); // 处理*操作符
    void optional(stack<int> &sta);  // 处理?操作符
    void positive_closure(stack<int> &sta);  // 处理+操作符

public:
    NFA(const string &input_str);
    void print_regexp();
    void print_splited_regexp();
    void print_postfix_regexp();
    void print_nfa();
    void export_to_png();
    void split_input_regexp(); // 分割正则表达式，插入连接符
    void build_postfix_regexp(); // 由正则表达式构建后缀表达式
    void build_nfa(); // 由后缀表达式构建NFA
};

#endif
#ifndef REMOVELEFTRECURSION_H
#define REMOVELEFTRECURSION_H

#include <bits/stdc++.h>
using namespace std;

#include "Production.h"

class RemoveLeftRecursion {
private:
    typedef vector<string> Tokens;
    Tokens all_tokens;                  // 存储所有输入符号
    Tokens non_terminals;               // 存储非终结符
    vector<bool> is_non_terminal;       // 标记某个符号是否为非终结符
    map<string,int> cp_idx;             // 将输入符号与内部编号相对应
    vector<string> idx_cp;              // 将内部编号与输入符号相对应
    vector<Production> productions;     // 存储所有产生式
    vector<Production> new_productions; // 存储消除左递归后的产生式

    void new_token(const string &token);
    void process_line(const string &line);
    Tokens utf8_tokens(const std::string& s);
    void output_production(const Production &prod, const string &title = "");
    void output_productions(const vector<Production> &prods, const string &title = "");

public:
    void input(const string &filename);
    void eliminate_left_recursion();
    void output_input_productions();
    void output_processed_productions();
};

#endif
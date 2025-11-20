#ifndef REMOVELEFTRECURSION_H
#define REMOVELEFTRECURSION_H

#include <bits/stdc++.h>
using namespace std;

#include "Production.h"

class RemoveLeftRecursion {
private:
    typedef vector<string> Tokens;
    Tokens all_tokens; // 存储所有输入符号
    Tokens non_terminals; // 存储非终结符
    vector<bool> is_non_terminal; // 标记某个符号是否为非终结符
    map<string,int> cp_idx; // 将输入符号与内部编号相对应
    map<int,string> idx_cp; // 将内部编号与输入符号相对应
    vector<Production> productions; // 存储所有产生式

    void new_token(const string &token);
    void process_line(const string &line);
    Tokens utf8_tokens(const std::string& s);

public:
    void input(const string &filename);
};

#endif
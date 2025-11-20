#ifndef REMOVELEFTRECURSION_H
#define REMOVELEFTRECURSION_H

#include <bits/stdc++.h>
using namespace std;

class RemoveLeftRecursion {
private:
    typedef vector<string> Tokens;
    map<string,int> cp_idx; // 将输入符号与内部编号相对应
    map<int,string> idx_cp; // 将内部编号与输入符号相对应
    vector<vector<vector<int>>> productions; // 产生式列表，按编号存储
        // productions[i]       存储编号为i的非终结符的所有产生式
        // productions[i][j]    存储编号为i的非终结符的第j条产生式
        // productions[i][j][k] 存储编号为i的非终结符的第j条产生式的第k个符号的编号

    void process_line(const string &line);
    Tokens utf8_tokens(const std::string& s);
    
public:
    void input(const string &filename);
};

#endif
#ifndef LEFTFACTORIZATION_H
#define LEFTFACTORIZATION_H

#include <bits/stdc++.h>
using namespace std;

#include "Production.h"

class LeftFactorization {
public:
    Productions input_prods;
    Productions new_prods;

public:
    void input(const string &filename);         // 从文件中读取产生式
    void output_input_productions();            // 输出输入的产生式
    void output_processed_productions();        // 输出处理后的产生式
    void eliminate_left_factorization();        // 消除左公因子
};

class TrieTree {
private:
    struct TrieNode {
        map<int, int> son;
        bool is_end;
        TrieNode() : is_end(false) {}
    };
    
    vector<TrieNode> nodes;
    const int root = 0; // 根节点编号，跟据设计，根节点编号恒为0

public:
    void build_from_productions(Productions &prods); // 从产生式构建Trie树
    void output_tree(Productions &prods, string title = "");
};


#endif
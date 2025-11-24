#ifndef LEFTFACTORIZATION_H
#define LEFTFACTORIZATION_H

#include <bits/stdc++.h>
using namespace std;

#include "Production.h"

class LeftFactorization {
public:
    Productions &input_prods;
    Productions new_prods;

public:
    LeftFactorization(Productions &productions) : input_prods(productions) {}
    void output_input_productions();            // 输出输入的产生式
    void output_processed_productions();        // 输出处理后的产生式
    void eliminate_left_factorization();        // 消除左公因子
};

class TrieTree {
    //TODO: 方便测试，记得删除
    // friend class LeftFactorization;
private:
    struct TrieNode {
        map<int, int> son;
        bool is_end;
        TrieNode() : is_end(false) {}
    };
    
    const int root = 0; // 根节点编号，跟据设计，根节点编号恒为0

    vector<TrieNode> nodes;
    Productions &prods, &target;

    void DFS(int p, Production prod);

public:
    TrieTree(Productions &prods, Productions &target) : prods(prods), target(target){}
    void build_from_productions(); // 从产生式构建Trie树
    void anslysis_trie();
    void output_tree(string title = "");
};


#endif
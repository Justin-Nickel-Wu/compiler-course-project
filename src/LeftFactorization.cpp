#include "LeftFactorization.h"

void LeftFactorization::input(const string &filename) {
    input_prods.input(filename);
}

void LeftFactorization::output_input_productions() {
    input_prods.output_productions("输入的产生式");
}

void LeftFactorization::output_processed_productions() {
    new_prods.output_productions("消除左公因子后的产生式");
}

void LeftFactorization::eliminate_left_factorization() {
    Productions prods;
    prods.init(input_prods);
    new_prods.init(input_prods);
    new_prods.clear();
    for (int i = 0; i < input_prods.size(); i++) {
        prods.push_back(input_prods[i]);
        if (i + 1 >= input_prods.size() || input_prods[i].lhs != input_prods[i + 1].lhs) {
            // 处理以同一非终结符为左部的产生式集合
            TrieTree trie(prods, new_prods);
            trie.build_from_productions();
            trie.output_tree("Trie-of-" + new_prods.get_token(prods[0].lhs));
            // Debug 输出
            // cout << "prods size: " << prods.size() << endl;
            // cout << "Trie size: " << trie.nodes.size() << endl;
            trie.anslysis_trie();
            prods.clear();
        }
    }
}

void TrieTree::build_from_productions() {
    nodes.clear();
    nodes.push_back(TrieNode()); // 根节点;

    for (auto prod: prods) {
        // if (prod.rhs.size() == 1 && prod.rhs[0] == prods.get_idx("ε"))
        //     continue; // 跳过 ε 产生式

        int p = root;
        for (auto j: prod.rhs) {
            if (nodes[p].son.find(j) == nodes[p].son.end()){
                nodes[p].son[j] = nodes.size(); // 新建节点
                nodes.push_back(TrieNode());
            }
            p = nodes[p].son[j];
        }

        nodes[p].is_end = true; // 标记产生式结束
    }
}

void TrieTree::output_tree(string title) {
    ofstream out_dot("./output/" + title +".dot");
    out_dot << "digraph Trie {\n";
    out_dot << "    rankdir=TB;\n";
    out_dot << "    node [shape=circle];\n";
    out_dot << "    label = \""<<title<<"\";\n";
    out_dot << "    labelloc = \"t\";\n";
    out_dot << "    fontsize = 20;\n";
    out_dot << "    fontname = \"Helvetica-Bold\";\n";
    out_dot << "\n";
    
    out_dot << "    {rank = min; 0; }\n";
    out_dot << "    0 [label=\"" + prods.get_token(prods[0].lhs) + "\"];\n";

    queue<int> que;
    que.push(root);
    while (!que.empty()){
        int p = que.front();
        que.pop();

        for (pair<int,int> son: nodes[p].son){
            // 输出节点
            out_dot << "    " << son.second << " [label=\""
                    << prods.get_token(son.first) << '\"';
            if (nodes[son.second].is_end)
                out_dot << ", peripheries=2";
            out_dot<< "];\n";
            // 输出边
            out_dot << "    " << p << " -> " << son.second << ";\n";
            que.push(son.second);
        }
    }
    out_dot << "}" << endl;
    out_dot.close();
    system(("dot -Tpng ./output/" + title + ".dot -o ./output/" + title + ".png").c_str());
}

void TrieTree::DFS(int p, Production prod) {
    if (nodes[p].son.empty()) { // 到达叶子结点
        target.push_back(prod);
        return;
    }

    if (nodes[p].son.size() > 1 || nodes[p].is_end) { // 存在分叉或有子串时需要提取
        string new_lhs = target.get_token(prod.lhs) + "'"; // 创建A‘
        while (target.get_idx(new_lhs) != -1)
            new_lhs += "'"; // A'被用过，就继续加'
        target.new_token(new_lhs);

        prod.rhs.push_back(target.get_idx(new_lhs)); // 变成A->xxxA'
        target.push_back(prod); // 增加新文法

        prod.lhs = target.get_idx(new_lhs); // 之后的文法变成A'->xxx
        prod.rhs.clear();

        if (nodes[p].is_end) { // 如果是trie树上标记了结束，要插入A'->ε
            prod.rhs.push_back(target.get_idx("ε"));
            target.push_back(prod);
            prod.rhs.pop_back();
        }
    }

    for (auto son: nodes[p].son) {
        int idx = son.first, q = son.second;
        prod.rhs.push_back(idx);
        DFS(q, prod);
        prod.rhs.pop_back();
    }
}

void TrieTree::anslysis_trie() {
    Production prod(prods[0].lhs);

    for (auto son: nodes[root].son) {
        prod.rhs.push_back(son.first);
        DFS(son.second, prod);
        prod.rhs.pop_back();
    }

    target.sort();
}
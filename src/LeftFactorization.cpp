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

void TrieTree::build_from_productions(Productions &prods) {
    nodes.clear();
    nodes.push_back(TrieNode()); // 根节点

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

void TrieTree::output_tree(Productions &prods, string title) {
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
            out_dot << "    " << son.second << " [label=\"" 
                    << prods.get_token(son.first) << "\"];\n";
            out_dot << "    " << p << " -> " << son.second << ";\n";
            que.push(son.second);
        }
    }
    out_dot << "}" << endl;
    out_dot.close();
    system(("dot -Tpng ./output/" + title + ".dot -o ./output/" + title + ".png").c_str());
}
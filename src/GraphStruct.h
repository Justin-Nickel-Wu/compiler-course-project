#ifndef GRAPHSTRUCT_H

#define GRAPHSTRUCT_H
#include <bits/stdc++.h>
using namespace std;

class GraphStruct{
private:
    struct NodeType{
        std::vector<int> alpha[26], e;
        bool is_final = 0;
    };
    vector<NodeType> g;
    int q0 = -1;

public:
    const NodeType void_node;
    void export_to_png(string title = "", bool is_nfa = 0); // 导出状态图为png图片
    void add_void_node(){ g.push_back(void_node);};
    int size(){ return g.size(); }
    int get_q0(){ return q0; }
    void make_q0(int x){ q0 = x; }
    bool is_final(int x){ return g[x].is_final; }
    void make_final(int x){ g[x].is_final = 1; }
    void add_edge(int from, int to, char w = '\0');
    vector<int>& to(int from, char w = '\0');
    void resize(int new_size){ g.resize(new_size, void_node); }
};

#endif
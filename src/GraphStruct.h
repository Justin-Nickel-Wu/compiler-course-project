#ifndef GRAPHSTRUCT_H

#define GRAPHSTRUCT_H
#include <bits/stdc++.h>
using namespace std;

class GraphStruct{
public:
    struct NodeType{
        std::vector<int> alpha[26], e;
        bool is_final = 0;
    };
    static void export_to_png(vector<NodeType>* input, int q0, int size, string title = "", bool is_nfa = 0); // 导出状态图为png图片
};

#endif
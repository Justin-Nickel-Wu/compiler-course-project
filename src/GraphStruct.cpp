#ifndef GRAPHSTRUCT_CPP

#define GRAPHSTRUCT_CPP
#include "GraphStruct.h"
#include <bits/stdc++.h>
using namespace std;

void GraphStruct::export_to_png(string title, bool is_nfa, bool debug){
    ofstream out_dot("./output/" + title +".dot");
    out_dot << "digraph NFA {\n";
    out_dot << "    rankdir=LR;\n";
    out_dot << "    node [shape=circle];\n";
    out_dot << "    label = \""<<title<<"\";\n";
    out_dot << "    labelloc = \"t\";\n";
    out_dot << "    fontsize = 20;\n";
    out_dot << "    fontname = \"Helvetica-Bold\";\n";
    out_dot << "\n";

    for (int i = 0; i < g.size(); i++){
        out_dot << "    " << i + 1 << " [label=\"" << i + 1 << "\"";
        if (g[i].is_final)
            out_dot << ", peripheries=2];\n";
        else
            out_dot << "];\n";
    }
    out_dot << "\n";

    out_dot << "    start [shape=point];\n";
    out_dot << "    start -> " << get_q0() + 1 << ";\n";
    out_dot << "\n";

    if (debug) { // 直接按照内部编号输出，方便调试
        for (int i = 0; i < g.size(); i++){
            for (int j = 0; j < 26; j++){
                for (auto t : g[i].alpha[j]){
                    out_dot << "    " << i + 1 << " -> " << t + 1 << " [label=\"" << char(j + 'a') << "\"];\n";
                }
            }
            if (is_nfa){
                for (auto t : g[i].e){
                    out_dot << "    " << i + 1 << " -> " << t + 1 << " [label=\"ε\"];\n";
                }
            }
        }
    } else { // 按照BFS序输出，保证图片美观
        queue<int> q;
        vector<int> idx(g.size(), 0);
        q.push(q0), idx[q0] = 1;
        int count = 1;
        while (!q.empty()){
            int i = q.front(); q.pop();
            for (int j = 0; j < 26; j++){
                for (auto t : g[i].alpha[j]){
                    if (idx[t] == 0){
                        idx[t] = ++count;
                        q.push(t);
                    }
                    out_dot << "    " << idx[i] << " -> " << idx[t] << " [label=\"" << char(j + 'a') << "\"];\n";
                }
            }
            if (is_nfa){
                for (auto t : g[i].e){
                    if (idx[t] == 0){
                        idx[t] = ++count;
                        q.push(t);
                    }
                    out_dot << "    " << idx[i] << " -> " << idx[t] << " [label=\"ε\"];\n";
                }
            }
        }
    }
    
    out_dot << "}" << endl;
    out_dot.close();

    system(("dot -Tpng ./output/" + title + ".dot -o ./output/" + title + ".png").c_str());
    // system(("rm ./output/" + title + ".dot").c_str());
    // system(("open ./output/" + title + ".png").c_str());
}

void GraphStruct::add_edge(int from, int to, char w){
    if (w == '\0')
        g[from].e.push_back(to);
    else
        g[from].alpha[w - 'a'].push_back(to);
}

vector<int>& GraphStruct::to(int from, char w){
    return w == '\0' ? g[from].e : g[from].alpha[w - 'a'];
}

#endif
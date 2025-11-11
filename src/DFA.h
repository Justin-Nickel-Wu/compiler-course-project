#ifndef DFA_CPP

#define DFA_CPP
#include "GraphStruct.h"
#include "NFA.h"
#include <bits/stdc++.h>
using namespace std;

class DFA {
private:
    typedef GraphStruct::NodeType DFA_node;
    typedef set<int> Closure; // 存储一个状态集合
    const DFA_node void_DFA_node;

    vector<DFA_node> *nfa;
    int nfa_size, nfa_q0;
    vector<DFA_node> dfa;
    int dfa_size, dfa_q0;
    
    void get_epsilon_closure(int s, Closure &S); // 由s开始推广epsilon闭包，避免每次扫描set结构

public:
    DFA(class NFA &nfa);
    void export_to_png();
    void build_dfa();
};

#endif
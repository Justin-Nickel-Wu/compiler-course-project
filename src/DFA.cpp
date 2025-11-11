#include "DFA.h"
#include "GraphStruct.h"
#include <bits/stdc++.h>
using namespace std;

void DFA::get_epsilon_closure(int s, Closure &S){
    // cout << "epsilon: " << s << ' ' << (*nfa)[s].e.size() << ' ' << (*nfa).size() << endl;
    for (auto i: (*nfa)[s].e){
        // cout << "! " << i <<endl;
        if (S.count(i) == 0){
            S.insert(i);
            get_epsilon_closure(i, S);
        }
    }
} 

DFA::DFA(class NFA &nfa){
    this->nfa = &nfa.nfa;
    this->nfa_size = nfa.nfa_size;
    this->nfa_q0 = nfa.q0;
}

void DFA::export_to_png(){
    GraphStruct::export_to_png(&dfa, dfa_q0, dfa_size, "DFA示意图");
}

void DFA::build_dfa(){
    map<Closure, int> idx; // 状态集合到DFA状态编号的映射
    idx.clear();
    queue<Closure> q;
    dfa_size = 0;

    // cout << "Step 1" << endl;
    // 初始e闭包
    Closure start_closure;
    start_closure.insert(nfa_q0);
    get_epsilon_closure(nfa_q0, start_closure);
    dfa_q0 = idx[start_closure] = dfa_size++;
    q.push(start_closure);

    dfa.push_back(void_DFA_node); // 添加第一个dfa节点，并判断是不是接受节点
    for (auto i: start_closure)
        if ((*nfa)[i].is_final){
            dfa[0].is_final = 1;
            break;
        }
    // cout << "Step 2" << endl;
    while (!q.empty()){
        Closure now = q.front();
        q.pop();
        int now_id = idx[now];
        // cout << "Now: " << now_id << endl;
        // for (auto i: now)
        //     cout << i << ",";
        // cout << endl;
        // cout << "Step 4" << endl;
        for (int j = 0; j < 26; j++){
            Closure next;
            for (auto i: now) // 构建新的闭包
                for (auto k: (*nfa)[i].alpha[j]){
                    next.insert(k);
                    get_epsilon_closure(k, next);
                }
            if (!next.empty() && idx.count(next) == 0){
                q.push(next);
                dfa.push_back(void_DFA_node); // 添加第一个dfa节点，并判断是不是接受节点
                for (auto i: next)
                    if ((*nfa)[i].is_final){
                        dfa[dfa_size].is_final = 1;
                        break;
                    }
                idx[next] = dfa_size++;
            }
            // cout << "Step 5" << endl;
            // cout << "Next: " << next.size() << endl;
            // for (auto i: next)
            //     cout << i << ",";
            // cout << endl;
            // 连边并标记最终节点
            if (!next.empty()){
                int next_id = idx[next];
                // cout << "next_id: " << next_id << endl;
                dfa[now_id].alpha[j].push_back(next_id); 
                // cout << now_id << "-" << char(j + 'a') << "->" << next_id << endl;
            }
        }
    }
}
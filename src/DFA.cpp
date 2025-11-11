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

void DFA::export_to_png_mini(){
    GraphStruct::export_to_png(&mini_dfa, mini_dfa_q0, mini_dfa_size, "最小化DFA示意图");
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

void DFA::minimize_dfa(){
    // cout << "Step1" << endl;
    vector<int> tag(dfa_size);
    int tag_nums = 1;
    for (int i = 0; i < dfa_size; i++)
        tag[i] = dfa[i].is_final ? 1 : 0;

    // 大致思路：枚举tag（相当于某个状态集合），找出它们对应哪些节点，放入S
    // 把S中和第一节点不等价的节点全部标记为新tag
    // 重复执行
    // cout << "Step2" << endl;
    bool changed = 1;
    while (changed){
        changed = 0;
        for (int t = 0; t <= tag_nums; t++){
            vector<int> S;
            for (int i = 0; i < dfa_size; i++)
                if (tag[i] == t)
                    S.push_back(i);
            if (S.size() <= 1) // 少于一个就不需要继续啦
                continue;

            set<int> first; // 找出第一个节点的转移情况
            for (int j = 0; j < 26; j++)
                for (auto k: dfa[S[0]].alpha[j])
                    first.insert(tag[k]);

            for (int i = 1; i < S.size(); i++){
                set<int> now;
                for (int j = 0; j < 26; j++)
                    for (auto k: dfa[S[i]].alpha[j])
                        now.insert(tag[k]);
                if (now != first){ // 不等价，标记为新tag
                    if (!changed){
                        ++tag_nums;
                        changed = 1;
                    }
                    tag[S[i]] = tag_nums;
                }
            }
            if (changed)
                break;
        }
    }


    // 开始连边
    mini_dfa.clear();
    mini_dfa_size = tag_nums + 1;
    mini_dfa_q0 = tag[dfa_q0];
    mini_dfa.resize(mini_dfa_size, void_DFA_node);
    // cout << "Step3" << endl;
    // cout << "Mini DFA Size: " << tag_nums + 1 << endl;
    set<pair<pair<int, int>, int>> exist_edge; // 去重边
    for (int i = 0; i < dfa_size; i++){
        if (dfa[i].is_final)
            mini_dfa[tag[i]].is_final = 1;
        for (int j = 0; j < 26; j++){
            for (auto k: dfa[i].alpha[j]){
                if (tag[i] != tag[k] && !exist_edge.count({{tag[i], tag[k]}, j})){
                    // cout << "Mini Edge: " << tag[i] << "-" << char(j + 'a') << "->" << tag[k] << endl;
                    // cout << "From " << i << " " << k << endl;
                    mini_dfa[tag[i]].alpha[j].push_back(tag[k]);
                    exist_edge.insert({{tag[i], tag[k]}, j});
                    // cout << "end" << endl;
                }
            }
        }
    }
}
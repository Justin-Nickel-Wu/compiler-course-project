#include "DFA.h"
#include "GraphStruct.h"
#include <bits/stdc++.h>
using namespace std;

void DFA::get_epsilon_closure(int s, Closure &S){
    // cout << "epsilon: " << s << ' ' << (*nfa)[s].e.size() << ' ' << (*nfa).size() << endl;
    for (auto i: nfa->to(s)){
        // cout << "! " << i <<endl;
        if (S.count(i) == 0){
            S.insert(i);
            get_epsilon_closure(i, S);
        }
    }
} 

DFA::DFA(class NFA &nfa){
    this->nfa = &nfa.nfa;
    this->dfa.make_q0(nfa.nfa.get_q0());
}

void DFA::export_to_png(){
    dfa.export_to_png("DFA示意图");
}

void DFA::export_to_png_mini(){
    mini_dfa.export_to_png("最小化DFA示意图");
}

void DFA::build_dfa(){
    map<Closure, int> idx; // 状态集合到DFA状态编号的映射
    idx.clear();
    queue<Closure> q;

    // cout << "Step 1" << endl;
    Closure start_closure;

    start_closure.insert(nfa->get_q0()); // 初始e闭包 
    get_epsilon_closure(nfa->get_q0(), start_closure);
    q.push(start_closure);

    dfa.add_void_node(); // 添加第一个dfa节点，并判断是不是接受节点
    dfa.make_q0(0);
    idx[start_closure] = 0;
    
    for (auto i: start_closure)
        if (nfa->is_final(i)){
            dfa.make_final(0);
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
                for (auto k: nfa->to(i, char(j + 'a'))){
                    next.insert(k);
                    get_epsilon_closure(k, next);
                }
            if (!next.empty() && idx.count(next) == 0){
                q.push(next);
                dfa.add_void_node(); // 添加第一个dfa节点，并判断是不是接受节点
                for (auto i: next)
                    if (nfa->is_final(i)){
                        dfa.make_final(dfa.size() - 1);
                        break;
                    }
                idx[next] = dfa.size() - 1;
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
                dfa.add_edge(now_id, next_id, char(j + 'a')); 
                // cout << now_id << "-" << char(j + 'a') << "->" << next_id << endl;
            }
        }
    }
}

void DFA::minimize_dfa(){
    // cout << "Step1" << endl;
    vector<int> tag(dfa.size());
    int tag_nums = 0;
    for (int i = 0; i < dfa.size(); i++)
        if (dfa.is_final(i)) // 存在所有节点都可接受的可能性！
            tag[i] = 0;
        else {
            tag[i] = 1;
            tag_nums = 1;
        }
    // 大致思路：枚举tag（相当于某个状态集合），找出它们对应哪些节点，放入S
    // 把S中和第一节点不等价的节点全部标记为新tag
    // 重复执行
    // cout << "Step2" << endl;
    bool changed = 1;
    while (changed){
        // cout << "tag" << endl;
        // for (int i = 0; i < dfa_size; i++)
        //     cout << i + 1 << ' ';
        // cout << endl;
        // for (int i = 0; i < dfa_size; i++)
        //     cout << tag[i] + 1 << ' ';
        // cout << endl;
        changed = 0;
        for (int t = 0; t <= tag_nums; t++){
            vector<int> S;
            for (int i = 0; i < dfa.size(); i++)
                if (tag[i] == t)
                    S.push_back(i);
            if (S.size() <= 1) // 少于一个就不需要继续啦
                continue;

            set<int> first; // 找出第一个节点的转移情况
            for (int j = 0; j < 26; j++)
                for (auto k: dfa.to(S[0], char(j + 'a')))
                    first.insert(tag[k]);

            for (int i = 1; i < S.size(); i++){
                set<int> now;
                for (int j = 0; j < 26; j++)
                    for (auto k: dfa.to(S[i], char(j + 'a')))
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

    mini_dfa.make_q0(tag[dfa.get_q0()]);
    mini_dfa.resize(tag_nums + 1);
    // cout << "Step3" << endl;
    // cout << "Mini DFA Size: " << tag_nums + 1 << endl;
    set<pair<pair<int, int>, int>> exist_edge; // 去重边
    for (int i = 0; i < dfa.size(); i++){
        if (dfa.is_final(i))
            mini_dfa.make_final(tag[i]);
        for (int j = 0; j < 26; j++){
            for (auto k: dfa.to(i, char(j + 'a'))){
                if (!exist_edge.count({{tag[i], tag[k]}, j})){
                    // cout << "Mini Edge: " << tag[i] << "-" << char(j + 'a') << "->" << tag[k] << endl;
                    // cout << "From " << i << " " << k << endl;
                    mini_dfa.add_edge(tag[i], tag[k], char(j + 'a'));
                    exist_edge.insert({{tag[i], tag[k]}, j});
                    // cout << "end" << endl;
                }
            }
        }
    }
}
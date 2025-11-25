#include "FirstFollowCalculator.h"

void FirstFollowCalculator::output_input_productions() {
    prods.output_productions("输入的产生式:");
}

void FirstFollowCalculator::calculate_first() {
    First.resize(prods.token_size());
    __have_epsilon.resize(prods.token_size(), false);

    for (auto token = prods.token_begin(); token != prods.token_end(); ++token) {
        int idx = prods.get_idx(*token);
        if (!prods.is_non_terminal(idx)) {
            First[idx].insert(idx);
            if (*token == "ε")
                __have_epsilon[idx] = true;
        }
    }

    bool changed;
    int epsilon_idx = prods.get_idx("ε");
    do {
        changed = false;
        for (auto prod: prods) { // 枚举每一条产生式
            bool all_have_epsilon = true;  // 用于判断某次遍历中是否所有非终结符都能推出ε

            for (auto token_idx: prod.rhs) { // 枚举产生式右部的每个符号
                for (auto symbol_idx: First[token_idx]) { // 枚举符号的First集
                    if (symbol_idx != epsilon_idx && First[prod.lhs].insert(symbol_idx).second) { // 插入成功，有修改。注意ε不需要加入
                        changed = true;
                    }
                }
                if (!__have_epsilon[token_idx]) { // 不包含ε，停止向后处理
                    all_have_epsilon = false;
                    break; 
                }
            }
            if (all_have_epsilon) {
                First[prod.lhs].insert(epsilon_idx); // 如果所有符号都能推出ε,则左部也能推出ε
                __have_epsilon[prod.lhs] = true;
            }
        }
    } while (changed);
}

void FirstFollowCalculator::calculate_follow() {
    Follow.resize(prods.token_size());
    int start_idx = prods.get_start_idx();

    prods.new_token("$"); // 添加输入结束符号
    Follow[start_idx].insert(prods.get_idx("$")); // 文法开始符号的Follow集加入输入结束符号

    bool changed;
    int epsilon_idx = prods.get_idx("ε");
    do {
        changed = false;
        for (auto prod: prods) { // 枚举每一条产生式
            bool all_have_epsilon = true;

            for (int i = prod.rhs.size() - 1; i >= 0; i--) { // 从后往前枚举右部
                if (i + 1 < prod.rhs.size()) { // A->Bβ 情况,把β的FIRST加入B的FOLLOW
                    for (auto symbol_idx: First[prod.rhs[i + 1]]) {
                        if (symbol_idx != epsilon_idx && Follow[prod.rhs[i]].insert(symbol_idx).second) {
                            // debug
                            // cout << "* 向Follow(" << prods.get_token(prod.rhs[i]) << ")中加入First(" << prods.get_token(prod.rhs[i + 1]) << ")的元素 " << prods.get_token(symbol_idx) << endl;
                            changed = true;
                        }
                    }
                }

                if (all_have_epsilon && prods.is_non_terminal(prod.rhs[i])) { // A->B情况,把A的FOLLOW加入B的FOLLOW
                    for (auto symbol_idx: Follow[prod.lhs]) {
                        if (symbol_idx != epsilon_idx && Follow[prod.rhs[i]].insert(symbol_idx).second) {
                            // debug
                            // cout << "* 向Follow(" << prods.get_token(prod.rhs[i]) << ")中加入Follow(" << prods.get_token(prod.lhs) << ")的元素 " << prods.get_token(symbol_idx) << endl;
                            changed = true;
                        }
                    }
                }

                if (!__have_epsilon[prod.rhs[i]])
                    all_have_epsilon = false;
            }
        }
    } while (changed);
}

void FirstFollowCalculator::output_first() {
    cout << "First集:" << endl;
    for (auto i = prods.non_terminal_begin(); i != prods.non_terminal_end(); ++i) {
        int idx = *i;
        cout << "   First(" << prods.get_token(idx) << ") = { ";
        bool first = true;
        for (auto symbol: First[idx]) {
            if (!first) cout << ", ";
            cout << prods.get_token(symbol);
            first = false;
        }
        cout << " }" << endl;
    }
    cout << endl;
}

void FirstFollowCalculator::output_follow() {
    cout << "Follow集:" << endl;
    for (auto i = prods.non_terminal_begin(); i != prods.non_terminal_end(); ++i) {
        int idx = *i;
        cout << "   Follow(" << prods.get_token(idx) << ") = { ";
        bool first = true;
        for (auto symbol: Follow[idx]) {
            if (!first) cout << ", ";
            cout << prods.get_token(symbol);
            first = false;
        }
        cout << " }" << endl;
    }
    cout << endl;
}

set<int>& FirstFollowCalculator::get_First(int symbol_idx) {
    return First[symbol_idx];
}

set<int>& FirstFollowCalculator::get_Follow(int symbol_idx) {
    return Follow[symbol_idx];
}

bool FirstFollowCalculator::have_epsilon(int idx) {
    return __have_epsilon[idx];
}
#include "FirstFollowCalculator.h"

void FirstFollowCalculator::output_input_productions() {
    prods.output_productions("输入的产生式:");
}

void FirstFollowCalculator::calculate_first() {
    First.resize(prods.token_size());
    vector<bool> have_epsilon(prods.token_size(), false);

    for (auto token = prods.token_begin(); token != prods.token_end(); ++token) {
        int idx = prods.get_idx(*token);
        if (!prods.is_non_terminal(idx)) {
            First[idx].insert(idx);
            if (*token == "ε")
                have_epsilon[idx] = true;
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
                if (!have_epsilon[token_idx]) { // 不包含ε，停止向后处理
                    all_have_epsilon = false;
                    break; 
                }
            }
            if (all_have_epsilon) {
                First[prod.lhs].insert(epsilon_idx); // 如果所有符号都能推出ε,则左部也能推出ε
                have_epsilon[prod.lhs] = true;
            }
        }
    } while (changed);
}

void FirstFollowCalculator::calculate_follow() {
    //TODO
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
   //TODO
}
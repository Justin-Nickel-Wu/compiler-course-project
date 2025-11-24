#include "GraphStruct.h"
#include "NFA.h"
#include "DFA.h"
#include "RemoveLeftRecursion.h"
#include "LeftFactorization.h"
#include "FirstFollowCalculator.h"
#include <bits/stdc++.h>
using namespace std;

string input_str;

void DFA_process(){
    while (1){
            cout << "请输入正则表达式 (输入 quit 结束): " << endl;
            cin >> input_str;
            if (input_str == "quit")
                break;

            NFA nfa(input_str);

            // nfa.print_regexp();
            nfa.split_input_regexp();
            // nfa.print_splited_regexp();
            nfa.build_postfix_regexp();
            // nfa.print_postfix_regexp();
            nfa.build_nfa();
            // nfa.print_nfa();
            nfa.export_to_png();

            DFA dfa(nfa);

            dfa.build_dfa();
            dfa.export_to_png();

            dfa.minimize_dfa();
            dfa.export_to_png_mini();

            cout << endl;
        }
}

void RLR_process(){
    Productions prods;
    RemoveLeftRecursion rlr(prods);

    prods.input("./grammar_RLR.in");
    rlr.output_input_productions();
    rlr.eliminate_left_recursion();
    rlr.output_processed_productions();
}

void LF_process(){
    Productions prods;
    LeftFactorization lf(prods);

    prods.input("./grammar_LF.in");
    lf.output_input_productions();
    cout << "开始消除左公因子...\n" << endl;
    lf.eliminate_left_factorization();
    lf.output_processed_productions();
}

void FF_process(){
    Productions prods;
    FirstFollowCalculator ff(prods);

    prods.input("./grammar_FF.in");
    ff.output_input_productions();
    ff.calculate_first();
    ff.output_first();
    ff.calculate_follow();
    ff.output_follow();
}

void Err(){
    cout << "输入参数错误!" << endl;
    cout << "用法：" << endl;
    cout << "make run DFA: 运行正则表达式到DFA的转换" << endl;
    cout << "make run RLR: 运行消除左递归" << endl;
    cout << "make run LF: 运行消除左公因子" << endl;
    cout << "make run FF: 运行First和Follow集计算" << endl;
    exit(1);
}

int main(int argc, char* argv[]){
    if (argc != 2) Err();
    else if (string(argv[1]) == "DFA" || string(argv[1]) == "dfa") DFA_process();
    else if (string(argv[1]) == "RLR" || string(argv[1]) == "rlr") RLR_process();
    else if (string(argv[1]) == "LF" || string(argv[1]) == "lf") LF_process();
    else if (string(argv[1]) == "FF" || string(argv[1]) == "ff") FF_process();
    else Err();

    return 0;
}

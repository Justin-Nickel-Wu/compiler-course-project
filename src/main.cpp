#include "GraphStruct.h"
#include "NFA.h"
#include "DFA.h"
#include "RemoveLeftRecursion.h"
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
    RemoveLeftRecursion rlr;
    rlr.input("./grammar.in");
    rlr.output_input_productions();
    rlr.eliminate_left_recursion();
    rlr.output_processed_productions();
}

void Err(){
    cout << "输入参数错误!" << endl;
    cout << "用法：" << endl;
    cout << "make run DFA: 运行正则表达式到DFA的转换" << endl;
    cout << "make run RLR: 运行消除左递归" << endl;
    exit(1);
}

int main(int argc, char* argv[]){
    if (argc != 2) Err();
    else if (string(argv[1]) == "DFA") DFA_process();
    else if (string(argv[1]) == "RLR") RLR_process();
    else Err();

    return 0;
}

#include "GraphStruct.h"
#include "NFA.h"
#include "DFA.h"
#include <bits/stdc++.h>
using namespace std;

string input_str;

int main(){
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

        cout << endl;
    }
    return 0;
}
#define DEFAULT_FILE "./input/LL1-SimpleExpr.in"

#include "GraphStruct.h"
#include "NFA.h"
#include "DFA.h"
#include "RemoveLeftRecursion.h"
#include "LeftFactorization.h"
#include "FirstFollowCalculator.h"
#include "LL1.h"
#include <bits/stdc++.h>
using namespace std;

string input_file;

void DFA_process(){
    while (1){
            cout << "请输入正则表达式 (输入 quit 结束): " << endl;
            getline(cin, input_file);
            if (input_file == "quit")
                break;

            NFA nfa(input_file);

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

    prods.input(input_file);
    rlr.output_input_productions();
    rlr.eliminate_left_recursion();
    rlr.output_processed_productions();
}

void LF_process(){
    Productions prods;
    LeftFactorization lf(prods);

    prods.input(input_file);
    lf.output_input_productions();
    cout << "开始消除左公因子...\n" << endl;
    lf.eliminate_left_factorization();
    lf.output_processed_productions();
}

void FF_process(){
    Productions prods;
    FirstFollowCalculator ff(prods);

    prods.input(input_file);
    ff.output_input_productions();
    ff.calculate_first();
    ff.output_first();
    ff.calculate_follow();
    ff.output_follow();
}

void LL1_process(){
    Productions prods;
    FirstFollowCalculator ff(prods);
    LL1 ll1(prods, ff);

    prods.input(input_file);
    if (ll1.bulid_parse_table()){
        cout << "\033[32mLL(1)分析表构建成功!\033[0m\n" << endl;
    }
    else {
        cout << "\033[31m输入的不是LL(1)文法,LL(1)分析表构建失败!\033[0m" << endl;
        return;
    }

    ll1.output_parse_table("LL1_parse_table");
    while (1) {
        cout << "========================================\n" << endl;
        cout << "请输入待匹配的字符串 (输入 quit 结束): " << endl;
        getline(cin, input_file);
        cout << endl;
        if (input_file == "quit")
            break;

        if (ll1.match(input_file)) {
            cout << "\033[32m字符串 " << input_file << " 匹配成功!\033[0m\n" << endl;
        }
        else {
            cout << "\033[31m字符串 " << input_file << " 匹配失败!\033[0m\n" << endl;
        }
    }
}

void Err(){
    cout << "输入参数错误!" << endl;
    cout << "用法：" << endl;
    cout << "make run DFA [文件名（可选）]: 运行正则表达式到DFA的转换" << endl;
    cout << "make run RLR [文件名（可选）]: 运行消除左递归" << endl;
    cout << "make run LF [文件名（可选）]: 运行消除左公因子" << endl;
    cout << "make run FF [文件名（可选）]: 运行First和Follow集计算" << endl;
    exit(1);
}

int main(int argc, char* argv[]){
    input_file = DEFAULT_FILE;
    if (argc == 3) {
        input_file = "./input/" + string(argv[2]);
        ifstream fin(input_file);
        if (!fin) {
            cout << "无法打开输入文件: /input/" << input_file << endl;
            cout << "请确保文件存在于 input 文件夹中。" << endl;
            exit(1);
        } else
            fin.close();
    }

    if (argc != 2 && argc != 3) Err();
    else if (string(argv[1]) == "DFA" || string(argv[1]) == "dfa") DFA_process();
    else if (string(argv[1]) == "RLR" || string(argv[1]) == "rlr") RLR_process();
    else if (string(argv[1]) == "LF" || string(argv[1]) == "lf") LF_process();
    else if (string(argv[1]) == "FF" || string(argv[1]) == "ff") FF_process();
    else if (string(argv[1]) == "LL1" || string(argv[1]) == "ll1") LL1_process();
    else Err();

    return 0;
}

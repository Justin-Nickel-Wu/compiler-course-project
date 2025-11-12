#include "GraphStruct.h"
#include "NFA.h"

#include <bits/stdc++.h>
using namespace std;

bool NFA::need_split(char ch1, char ch2){
        //  a * + ? | ( )
        //a T         T
        //* T         T
        //+ T         T
        //? T         T    
        //|       
        //( 
        //) T         T
        return ch1 != '|' && ch1 != '('
            && ch2 != '*' && ch2 != '+' && ch2 != '?' && ch2 != '|' && ch2 != ')';
    }

int NFA::op_priority(char op){
    switch (op){
        case '*' : return 3;
        case '?' : return 3;
        case '+' : return 3;
        case '.' : return 2;
        case '|' : return 1;
        default  : return 0; // 括号优先级最低
    }
}

void NFA::create_symbol(stack<int> &sta, char ch){
    nfa.add_void_node();
    nfa.add_void_node();
    nfa.add_edge(nfa.size() - 2, nfa.size() - 1, ch); // 连边

    sta.push(nfa.size() - 2);
    sta.push(nfa.size() - 1);
}

void NFA::union_(stack<int> &sta){
    int a, b, x, y;
    y = sta.top(), sta.pop();
    x = sta.top(), sta.pop();
    b = sta.top(), sta.pop();
    a = sta.top(), sta.pop();

    nfa.add_void_node();
    nfa.add_void_node();

    nfa.add_edge(nfa.size() - 2, x);
    nfa.add_edge(nfa.size() - 2, a);
    nfa.add_edge(y, nfa.size() - 1);
    nfa.add_edge(b, nfa.size() - 1);

    sta.push(nfa.size() - 2);
    sta.push(nfa.size() - 1);
}

void NFA::concat(stack<int> &sta){
    int a, b, x, y;
    y = sta.top(), sta.pop();
    x = sta.top(), sta.pop();
    b = sta.top(), sta.pop();
    a = sta.top(), sta.pop();

    nfa.add_edge(b, x); // 注意顺序
    sta.push(a);
    sta.push(y);
}

void NFA::closure(stack<int> &sta){
    int a, b;
    b = sta.top(), sta.pop();
    a = sta.top(), sta.pop();

    nfa.add_void_node();
    nfa.add_edge(b, a);
    nfa.add_edge(nfa.size() - 1, a);
    nfa.add_edge(nfa.size() - 1, b);
    sta.push(nfa.size() - 1);
    sta.push(b);
}

void NFA::optional(stack<int> &sta){
    int a, b;
    b = sta.top(), sta.pop();
    a = sta.top(), sta.pop();
    nfa.add_edge(a, b);
    sta.push(a);
    sta.push(b);
}

void NFA::positive_closure(stack<int> &sta){
    int a, b;
    b = sta.top(), sta.pop();
    a = sta.top(), sta.pop();
    nfa.add_edge(b, a);
    sta.push(a);
    sta.push(b);
}

NFA::NFA(const string &input_str){
    regexp = input_str;
    splited_regexp.clear();
}

void NFA::print_regexp(){
    cout << "输入正则表达式: " << regexp << endl;
}

void NFA::print_splited_regexp(){
    cout << "分割后的表达式: ";
    for (auto ch : splited_regexp)
        cout << ch;
    cout << endl;
}

void NFA::print_postfix_regexp(){
    cout << "后缀表达式: ";
    for (auto ch : postfix_regexp)
        cout << ch;
    cout << endl;
}

void NFA::print_nfa(){
    cout << "NFA States: " << nfa.size() << endl;
    for (int i = 0; i < nfa.size(); i++){
        cout << "State " << i << ": ";
        for (int j = 0; j < 26; j++){
            for (auto t : nfa.to(i, char(j + 'a')))
                cout << "'" << char(j + 'a') << "' -> " << t << "  ";
        }
        for (auto t : nfa.to(i))
            cout << "ε -> " << t << "  ";
        cout << endl;
    }
}

void NFA::export_to_png(){
    nfa.export_to_png("NFA示意图", 1);
}

void NFA::split_input_regexp(){
    char nowc, nextc;
    for (int i = 0; i < regexp.size(); i++){
        nowc = regexp[i];
        splited_regexp.push_back(nowc);
        if (i + 1 < regexp.size()){
            nextc = regexp[i + 1];
            if (need_split(nowc, nextc))
                splited_regexp.push_back('.');
        }
    }
}
    
void NFA::build_postfix_regexp(){
    stack<char> op_stack;
    postfix_regexp.clear();

    for (auto ch : splited_regexp)
        if (isalpha(ch)){
            postfix_regexp.push_back(ch);
        }
        else if (ch == '('){
            op_stack.push(ch);
        }
        else if (ch == ')'){ 
            // 此时括号内操作符的优先级别一定是递增的，从往前算
            while (op_stack.top() != '('){
                postfix_regexp.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.pop();
        }
        else { // * 或 ｜
            while (!op_stack.empty() && op_priority(op_stack.top()) >= op_priority(ch)){
                // 保证栈内优先级递增，即需要从后往前计算
                postfix_regexp.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.push(ch);
        }
    // 处理残余操作符
    while (!op_stack.empty()){
        postfix_regexp.push_back(op_stack.top());
        op_stack.pop();
    }
}

void NFA::build_nfa(){
    stack<int> sta;
    for (auto ch : postfix_regexp){
        // cout << "! " << ch << endl;
        switch (ch){
            case '*': closure(sta); break;
            case '?': optional(sta); break;
            case '+': positive_closure(sta); break;
            case '.': concat(sta); break;
            case '|': union_(sta); break;
            default: create_symbol(sta, ch);
        }
    }
    int a, b;
    b = sta.top(), sta.pop();
    a = sta.top(), sta.pop();
    nfa.make_final(b); // 标记终态
    nfa.make_q0(a);
}
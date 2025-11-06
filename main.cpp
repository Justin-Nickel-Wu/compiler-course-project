#include <bits/stdc++.h>
using namespace std;

string input_str;

class NFA {
private:
    string regexp;
    vector<char> splited_regexp;
    vector<char> postfix_regexp;

    // 判断是否需要在两个字符之间插入连接符
    bool need_split(char ch1, char ch2){
        // aa *_  )a _(
        return (isalpha(ch1) && isalpha(ch2))
                || (ch1 == '*') 
                || (ch1 == ')' && isalpha(ch2))
                || (ch2 == '(');
    }

public:
    NFA(const string &input_str){
        regexp = input_str;
        splited_regexp.clear();
    }

    void print_regexp(){
        cout << "Input Expression: " << regexp << endl;
    }

    void print_splited_regexp(){
        cout << "Splited Expression: ";
        for (auto ch : splited_regexp)
            cout << ch;
        cout << endl;
    }

    void print_postfix_regexp(){
        cout << "Postfix Expression: ";
        for (auto ch : postfix_regexp)
            cout << ch;
        cout << endl;
    }

    // 分割正则表达式，插入连接符
    void split_input_regexp(){
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

    int op_priority(char op){
        switch (op){
            case '*' : return 3;
            case '.' : return 2;
            case '|' : return 1;
            default  : return 0; // 括号优先级最低
        }
    }
    
    // 由正则表达式构建后缀表达式
    void build_postfix_regexp(){
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
};

int main(){
    freopen("in.txt", "r", stdin);
    freopen("out.txt", "w", stdout);
    while (cin >> input_str){
        NFA nfa(input_str);

        nfa.print_regexp();

        nfa.split_input_regexp();
        nfa.print_splited_regexp();

        nfa.build_postfix_regexp();
        nfa.print_postfix_regexp();

        cout << endl;
    }
    return 0;
}
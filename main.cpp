#include <bits/stdc++.h>
using namespace std;

string input_str;

class NFA {
private:
    string regexp;
    vector<char> split_regexp;

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
        split_regexp.clear();
    }

    void print_split_regexp(const string &regexp){
        for (auto ch : split_regexp)
            cout << ch;
        cout << endl;
    }

    // 分割正则表达式，插入连接符
    void split_input_regexp(){
        char nowc, nextc;
        for (int i = 0; i < regexp.size(); i++){
            nowc = regexp[i];
            split_regexp.push_back(nowc);
            if (i + 1 < regexp.size()){
                nextc = regexp[i + 1];
                if (need_split(nowc, nextc))
                    split_regexp.push_back('.');
            }
        }
    }
};

int main(){
    freopen("in.txt", "r", stdin);
    freopen("out.txt", "w", stdout);
    while (cin >> input_str){
        NFA nfa(input_str);
        nfa.split_input_regexp();
        nfa.print_split_regexp(input_str);
    }
    return 0;
}
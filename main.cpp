#include <bits/stdc++.h>
using namespace std;

string input_str;


class Node{
public:
    struct NodeType{
        vector<int> alpha[26], e;
        bool is_final = 0;
    };
};

class NFA {
    
    friend class DFA;

private:

    typedef Node::NodeType NFA_node;
    const NFA_node void_NFA_node;

    int q0; // 起始状态

    string regexp;               // 原始正则表达式
    vector<char> splited_regexp; // 插入连接符后的正则表达式
    vector<char> postfix_regexp; // 后缀表达式

    vector<NFA_node> nfa;
    int nfa_size;

    // 判断是否需要在两个字符之间插入连接符
    bool need_split(char ch1, char ch2){
        // aa *_  )a _(
        return (isalpha(ch1) && isalpha(ch2))
                || ((ch1 == '*') && ch2 != ')' && ch2 != '|') 
                || (ch1 == ')' && isalpha(ch2))
                || (ch2 == '(');
    }

    int op_priority(char op){
        switch (op){
            case '*' : return 3;
            case '.' : return 2;
            case '|' : return 1;
            default  : return 0; // 括号优先级最低
        }
    }

    // 添加一个字符的NFA片段
    void create_symbol(stack<int> &sta, char ch){
        nfa.push_back(void_NFA_node);
        nfa.push_back(void_NFA_node);
        nfa[nfa_size].alpha[ch - 'a'].push_back(nfa_size + 1); // 连边

        sta.push(nfa_size), nfa_size++;
        sta.push(nfa_size), nfa_size++;
    }

    // 处理｜操作符
    void union_(stack<int> &sta){
        int a, b, x, y;
        y = sta.top(), sta.pop();
        x = sta.top(), sta.pop();
        b = sta.top(), sta.pop();
        a = sta.top(), sta.pop();

        nfa.push_back(void_NFA_node);
        nfa.push_back(void_NFA_node);

        nfa[nfa_size].e.push_back(x);
        nfa[nfa_size].e.push_back(a);
        nfa[y].e.push_back(nfa_size + 1);
        nfa[b].e.push_back(nfa_size + 1);

        sta.push(nfa_size), nfa_size++;
        sta.push(nfa_size), nfa_size++;
    }

    // 处理.操作符
    void concat(stack<int> &sta){
        int a, b, x, y;
        y = sta.top(), sta.pop();
        x = sta.top(), sta.pop();
        b = sta.top(), sta.pop();
        a = sta.top(), sta.pop();

        nfa[b].e.push_back(x); // 注意顺序
        sta.push(a);
        sta.push(y);
    }

    // 处理*操作符
    void closure(stack<int> &sta){
        int a, b;
        b = sta.top(), sta.pop();
        a = sta.top(), sta.pop();

        nfa.push_back(void_NFA_node);

        nfa[b].e.push_back(a);
        nfa[nfa_size].e.push_back(a);
        nfa[nfa_size].e.push_back(b);

        sta.push(nfa_size), nfa_size++;
        sta.push(b);
    }

public:

    NFA(const string &input_str){
        regexp = input_str;
        splited_regexp.clear();
    }

    void print_regexp(){
        cout << "输入正则表达式: " << regexp << endl;
    }

    void print_splited_regexp(){
        cout << "分割后的表达式: ";
        for (auto ch : splited_regexp)
            cout << ch;
        cout << endl;
    }

    void print_postfix_regexp(){
        cout << "后缀表达式: ";
        for (auto ch : postfix_regexp)
            cout << ch;
        cout << endl;
    }

    void print_nfa(){
        cout << "NFA States: " << nfa_size << endl;
        for (int i = 0; i < nfa_size; i++){
            cout << "State " << i << ": ";
            for (int j = 0; j < 26; j++){
                for (auto t : nfa[i].alpha[j])
                    cout << "'" << char(j + 'a') << "' -> " << t << "  ";
            }
            for (auto t : nfa[i].e)
                cout << "ε -> " << t << "  ";
            cout << endl;
        }
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

    // 由后缀表达式构建NFA
    void build_nfa(){
        stack<int> sta;
        nfa.clear();
        nfa_size = 0;
        for (auto ch : postfix_regexp){
            cout << "! " << ch << endl;
            switch (ch){
                case '*': closure(sta); break;
                case '.': concat(sta); break;
                case '|': union_(sta); break;
                default: create_symbol(sta, ch);
            }
        }

        int a, b;
        b = sta.top(), sta.pop();
        a = sta.top(), sta.pop();
        nfa[b].is_final = 1; // 标记终态
        q0 = a;
    }

    void export_to_png(){
        ofstream out_dot("nfa.dot");
        out_dot << "digraph NFA {\n";
        out_dot << "    rankdir=LR;\n";
        out_dot << "    node [shape=circle];\n";
        out_dot << "   label = \"" + regexp + " NFA示意图\";\n";
        out_dot << "   labelloc = \"t\";\n";
        out_dot << "   fontsize = 20;\n";
        out_dot << "   fontname = \"Helvetica-Bold\";\n";
        out_dot << "\n";

        for (int i = 0; i < nfa_size; i++){
            out_dot << "    " << i + 1 << " [label=\"" << i + 1 << "\"";
            if (nfa[i].is_final)
                out_dot << ", peripheries=2];\n";
            else
                out_dot << "];\n";
        }
        out_dot << "\n";

        out_dot << "    start [shape=point];\n";
        out_dot << "    start -> " << q0 + 1 << ";\n";
        out_dot << "\n";

        for (int i = 0; i < nfa_size; i++){
            for (int j = 0; j < 26; j++){
                for (auto t : nfa[i].alpha[j]){
                    out_dot << "    " << i + 1 << " -> " << t + 1 << " [label=\"" << char(j + 'a') << "\"];\n";
                }
            }
            for (auto t : nfa[i].e){
                out_dot << "    " << i + 1 << " -> " << t + 1 << " [label=\"ε\"];\n";
            }
        }
        out_dot << "}" << endl;
        out_dot.close();

        system("dot -Tpng nfa.dot -o nfa.png");
        // system("rm nfa.dot");
        system("open nfa.png");
    }
};

class DFA {
private:
    typedef Node::NodeType DFA_node;
    typedef set<int> Closure; // 存储一个状态集合

    vector<DFA_node> *nfa;
    int nfa_size, nfa_q0;
    vector<DFA_node> dfa;
    int dfa_size;

    // 由s开始推广epsilon闭包，避免每次扫描set结构
    void get_epsilon_closure(int s, Closure &S){
        for (auto i: (*nfa)[s].e)
            if (S.count(i) == 0){
                S.insert(i);
                get_epsilon_closure(i, S);
            }
    } 

public:
    DFA(class NFA nfa){
        this->nfa = &nfa.nfa;
        this->nfa_size = nfa.nfa_size;
        this->nfa_q0 = nfa.q0;
    }

    void build_dfa(){
        map<Closure, int> idx; // 状态集合到DFA状态编号的映射
        idx.clear();
        queue<Closure> q;
        int dfa_size = 0;

        // 初始e闭包
        Closure start_closure;
        get_epsilon_closure(nfa_q0, start_closure);
        idx[start_closure] = ++dfa_size;
        q.push(start_closure);

        while (!q.empty()){
            Closure now = q.front();
            q.pop();
            int now_id = idx[now];

            for (int j = 0; j < 26; j++){
                Closure next;
                for (auto i: now) // 构建新的闭包
                    for (auto k: (*nfa)[i].alpha[j]){
                        next.insert(k);
                        get_epsilon_closure(k, next);
                    }
                if (!next.empty() && idx.count(next) == 0){
                    q.push(next);
                    idx[next] = ++dfa_size;
                }

                // 连边并标记最终节点
                if (!next.empty()){
                    int next_id = idx[next];
                    dfa[now_id].alpha[j].push_back(next_id);

                    dfa[next_id].is_final = 0;
                    for (auto i: next)
                        if ((*nfa)[i].is_final){
                            dfa[next_id].is_final = 1;
                            break;
                        }
                }
            }
        }
    }

};

int main(){
    while (1){
        cout << "请输入正则表达式 (输入 quit 结束): " << endl;
        cin >> input_str;
        if (input_str == "quit")
            break;

        NFA nfa(input_str);

        nfa.print_regexp();

        nfa.split_input_regexp();
        nfa.print_splited_regexp();

        nfa.build_postfix_regexp();
        nfa.print_postfix_regexp();

        nfa.build_nfa();
        nfa.print_nfa();

        nfa.export_to_png();

        cout << endl;
    }
    return 0;
}
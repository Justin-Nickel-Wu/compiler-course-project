#include<bits/stdc++.h>
using namespace std;

//nfa结构定义
struct nst {
    vector<int> a[26], e; //接收a-z会到达的状态，接收eps会到达的状态
    bool f = 0; //=0为可接受态
};
vector<nst> nfa;
set<char>alp;
string str;
set<int>accepted;

struct dst {
    vector<int> a[26]; //int a[26] a[0] = 5
    bool f = 0; //1为可接受态
};
vector<dst> dfa;

stack<int> st;
int nfa_size = 0, dfa_size;
string dispregex;
struct nst init_nfa_state;
struct dst init_dfa_state;

//产生式定义
struct production{
    int a, b, c;
};

/***************************** regex to nfa ****************************/

//把单词隔开
string insert_concat(string regexp) {
    string ret = "";
    char c, c2;
    for (unsigned int i = 0; i < regexp.size(); i++) {
        c = regexp[i];
        if (i + 1 < regexp.size()) {
            c2 = regexp[i + 1];
            ret += c;
            if (c != '(' && c2 != ')' && c != '|' && c2 != '|' && c2 != '*') {
                ret += '.';
            }
        }
    }
    ret += regexp[regexp.size() - 1];
    return ret;
}

//输出nfa
void print_nfa() {
//    cout << "------------------------------------------------------------------------\n";
    cout << str << ":\n";
    cout << "正则式 转 NFA: \n";
    cout << "K = {";
    for (int i = 0; i < nfa.size(); i++) {
        if (i)  cout << ", ";
        cout << char(i + 'A');
    }
    cout << "};\n";
    cout << "Σ = {";
    auto it = alp.begin();
    cout << *it, it++;
    for (; it != alp.end(); it++) {
        cout << ", " << *it;
    }
    cout << "};\n";

    vector<production> ans;
    set<int> in; //记录有入度的点
    for (int i = 0; i < nfa.size(); i++) {
        bool &f = nfa[i].f;
        for (int j = 0; j < 26; j++) {
            if (nfa[i].a[j].size() == 0)    continue;
            f |= 1;
            for (auto k : nfa[i].a[j])  ans.push_back({i, j, k}), in.insert(k);
        }
        if (nfa[i].e.size() == 0)   continue;
        for (auto j : nfa[i].e)     ans.push_back({i, -1, j}), in.insert(j);
        f |= 1;
    }

    for (int i = 0; i < ans.size(); i++) {
        if (i)  cout << ", ";
        cout << "f(";
        cout << char(ans[i].a + 'A');
        cout << ", ";
        if (ans[i].b == -1) cout << "ε";
        else    cout << char(ans[i].b + 'a');
        cout << ") = ";
        cout << char(ans[i].c + 'A');
    }
    cout << ";\n";

    //没有入度就是起点
    for (int i = 0; i < nfa.size(); i++) {
        if (!in.count(i)) {
            cout << char(i + 'A') << ";\n";
            break;
        }
    }

    cout << "Z = {";
    vector<int> final;
    for (int i = 0; i < nfa.size(); i++) {
        if (!nfa[i].f)    final.push_back(i), accepted.insert(i);
    }
    for (int i = 0; i < final.size(); i++) {
        if (i)  cout << ", ";
        cout << char(final[i] + 'A');
    }
    cout << "};\n";
    cout << "..........................................\n";
}

//处理字母
void character(int i) {
    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);
    nfa[nfa_size].a[i].push_back(nfa_size + 1);
    st.push(nfa_size);
    //cout << char(i + 'a') << ' ' << nfa_size << ' ';
    nfa_size++;
    st.push(nfa_size);
    //cout << nfa_size << endl;
    nfa_size++;
    //print_nfa();
}

//处理'|'
void union_() {
    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);

    int d = st.top();
    st.pop();
    int c = st.top();
    st.pop();
    int b = st.top();
    st.pop();
    int a = st.top();
    st.pop();
    //cout << "| " << a << ' ' << b << ' ' << c << ' ' << d << endl;

    nfa[nfa_size].e.push_back(a);
    nfa[nfa_size].e.push_back(c);
    st.push(nfa_size);
    nfa_size++;
    nfa[b].e.push_back(nfa_size);
    nfa[d].e.push_back(nfa_size);
    st.push(nfa_size);
    nfa_size++;

    //print_nfa();
}

//处理'.'
void concatenation() {
    int d = st.top();
    st.pop();
    int c = st.top();
    st.pop();
    int b = st.top();
    st.pop();
    int a = st.top();
    st.pop();
    //cout << ". " << a << ' ' << b << ' ' << c << ' ' << d << endl;
    nfa[b].e.push_back(c);
    //nfa_size++;
    st.push(a);
    st.push(d);

    //print_nfa();
}

//处理'*'
void kleene_star() {
    //取出前两个
    //cout << st.size() << endl;
    int b = st.top();
    st.pop();
    int a = st.top();
    st.pop();
    //cout << "* " << a << ' ' << b << endl;
    //cout << nfa_size << endl;
    //再加三条边
    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);
    nfa[b].e.push_back(a);
    nfa[nfa_size].e.push_back(a);
    nfa[nfa_size].e.push_back(nfa_size + 1);
    nfa[b].e.push_back(nfa_size + 1);
    st.push(nfa_size);
    //cout << "** " << nfa_size << ' ';
    nfa_size++;
    st.push(nfa_size);
    //cout << nfa_size << endl;
    nfa_size++;
    //print_nfa();
    //cout << "------------------------------------\n";
}

//后缀转nfa
void postfix_to_nfa(string postfix) {
    for (unsigned int i = 0; i < postfix.size(); i++) {
        char ch = postfix[i];
        if (ch <= 'z' && ch >= 'a')     character(ch - 'a');
        else if (ch == '*')     kleene_star();
        else if (ch == '.')     concatenation();
        else if (ch == '|')     union_();
        else {
            cout << "输入为非法字符！读入只能是 a-z, |, *, ()" << endl;
        }
    }
}

//出入栈优先级
int priority(char c) {
    switch (c) {
        case '*':
            return 3;
        case '.':
            return 2;
        case '|':
            return 1;
        default:
            return 0;
    }
}

//正则式转后缀表达式
string regexp_to_postfix(string regexp) {
    string postfix = "";
    stack<char> op;
    char c;
    for (unsigned int i = 0; i < regexp.size(); i++) {
        char ch = regexp[i];
        if (ch <= 'z' && ch >= 'a')     postfix += ch;
        else if (ch == '(')     op.push(ch);
        else if (ch == ')') {
            while (op.top() != '(') {
                postfix += op.top();
                op.pop();
            }
            op.pop();
        }
        else {
            while (!op.empty()) {
                c = op.top();
                if (priority(c) >= priority(ch)) {
                    postfix += op.top();
                    op.pop();
                } else break;
            }
            op.push(regexp[i]);
        }
    }
    while (!op.empty()) {
        postfix += op.top();
        op.pop();
    }
    return postfix;
}

/***************************** nfa to dfa ****************************/

void print_dfa() {
    //cout << dfa_size << endl;
    dfa_size++;
    //cout << dfa_size << endl;
    cout << "NFA 转 DFA: " << endl;
    cout << "K = {";
    for (int i = 0; i < dfa_size; i++) {
        if (i)  cout << ", ";
        cout << i;
    }
    cout << "};\n";

    int fst = 0; //是否为第一次输出
    for (int i = 0; i < dfa_size; i++) {
        for (int j = 0; j < 26; j++) {
            if (dfa[i].a[j].size() == 0)    continue;
            for (auto k : dfa[i].a[j]) {
                if (fst)    cout << ", ";
                else    fst = 1;
                cout << "f(" << i << ", " << char(j + 'a') << ") = " << k;
            }
        }
    }
    cout << ";\n";

    cout << "Z = {";
    vector<int> final;
    for (int i = 0; i < dfa_size; i++) {
        if (dfa[i].f)   final.push_back(i);
    }
    for (int i = 0; i < final.size(); i++) {
        if (i)  cout << ", ";
        cout << final[i];
    }
    cout << "};\n";
    cout << "------------------------------------------------------------------------\n";
}

void epsilon_closure(int state, set<int> &si) {
    for (unsigned int i = 0; i < nfa[state].e.size(); i++) {
        if (si.count(nfa[state].e[i]) == 0) {
            si.insert(nfa[state].e[i]);
            epsilon_closure(nfa[state].e[i], si);
        }
    }
}

//打印闭包（调试）
void print_epsilon(int n, set<int>eps[]) {
    cout << "===========================\n";
    for (int i = 0; i < n; i++) {
        cout << i << ": ";
        for (auto j: eps[i]) cout << j << ' ';
        cout << endl;
    }
    cout << "===========================\n";
}

//nfa转dfa
void nfa_to_dfa(int start_state, int n) {
    dfa.resize(n);
    map<set<int>, int> mp, idx; //记录所有出现过的状态, state对应的状态数字
    set<int>si; //起始状态集

    //求所有状态的空闭包
    set<int> eps[n];
    for (int i = 0; i < n; i++)     epsilon_closure(i, eps[i]);
    si = eps[start_state];
    si.insert(start_state); //至少得有一个起始状态哦
    idx[si] = 0;
    //print_epsilon(n, eps);

    queue<set<int>> q;
    q.push(si);
    while (!q.empty()) {
        auto ss = q.front();
        q.pop();
        mp[ss]++;
        if (mp[ss] > 1) continue;

//        cout << "ss: ";
//        for (auto j : ss)   cout << j << ' ';
//        cout << endl;

        //接下来算第一行：起始状态先读入一个对应字符再做eps闭包
        for (auto ch: alp) { //第几个字符
            set<int> state; //记录状态
            int i = ch - 'a';
            for (auto st: ss) { //对应起点
                //st通过一个i边能到达的集合
                for (auto j: nfa[st].a[i]) {
                    state.insert(j); //首先是自己
                    //加上j的空闭包
                    for (auto k: eps[j]) state.insert(k);
                }
            }
            if (state.size() && mp[state] == 0) q.push(state);
            if (state.size()) {
                //cout << i << ": ";
                if (!idx.count(state))  dfa_size++, idx[state] = dfa_size;
                dfa[idx[ss]].a[i].push_back(idx[state]);
                //cout << "ATTENTION!!! " << idx[ss] << ' ' << idx[state] << endl;
                for (auto j : state) {
                    //cout << j << ' ';
                    if (accepted.count(j)) {
                        dfa[idx[state]].f = 1;
                        //cout << "& " << idx[state] << "\n";
                        break;
                    }
                }
                //cout << endl;
            }
        }
//        cout << "+++++++++++++++++++++++\n";
    }
}

/***************************** solve ****************************/

//每组处理前的清空
void clear() {
    nfa_size = dfa_size = 0;
    while(!st.empty())  st.pop();
    alp.clear();
    nfa.clear();
    dfa.clear();
}

//判断是否有非法输入
bool check (string postfix) {
    for (unsigned int i = 0; i < postfix.size(); i++) {
        char ch = postfix[i];
        if (ch <= 'z' && ch >= 'a') continue;
        else if (ch == '*')    continue;
        else if (ch == '.')    continue;
        else if (ch == '|')    continue;
        cout << "输入为非法字符！读入只能是: 小写英文字母, |, *, ()" << endl;
        return false; //检测到非法字符
    }
    return true;
}

void solve() {
    clear();
    string regexp, postfix;
    regexp = str;
    for (auto i : regexp) {
        if (i >= 'a' && i <= 'z')   alp.insert(i);
    }

    dispregex = regexp;
    regexp = insert_concat(regexp);
    cout << regexp << endl;
    postfix = regexp_to_postfix(regexp);
    cout << "Postfix Expression: " << postfix << endl;
    bool suc = check(postfix);
    if (suc) {
        postfix_to_nfa(postfix);
        print_nfa();
    }
    //cout << nfa_size << endl;
    //nfa_to_dfa(nfa_size);

    //开始转dfa
    int final_state = st.top();
    st.pop();
    int start_state = st.top();
    st.pop();
    //cout << start_state << ' ' << final_state << endl;
    nfa[final_state].f = 1;
    nfa_to_dfa(start_state, nfa.size());
    print_dfa();

    cout << endl << endl;

}

int main() {
    freopen("in.txt", "r", stdin);
    freopen("out_example.txt", "w", stdout);
    while (cin >> str) {
        solve();
    }
}

//正则式 -> 有空nfa -> 去空nfa -> dfa -> 极小化dfa

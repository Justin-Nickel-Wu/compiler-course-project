#include "RemoveLeftRecursion.h"

void RemoveLeftRecursion::new_token(const string &token){
    all_tokens.push_back(token);
    cp_idx[token] = cp_idx.size();
    idx_cp[cp_idx[token]] = token;
    is_non_terminal.push_back(false);
}

void RemoveLeftRecursion::RemoveLeftRecursion::process_line(const string &line){
    Tokens tokens = utf8_tokens(line);
    // debug输出分割结果
    // for (auto i: tokens)
    //     cout << "[" << i << "] ";
    // cout << endl;
    bool arrow_found = false;
    Production prod;
    for (auto i: tokens){
        if (i == "→")
            arrow_found = true;
        else if (i == "|"){
            productions.push_back(prod); // 新增一条产生式
            prod.rhs.clear();
        }
        else if (!arrow_found){ // 处理产生式左部
            if (cp_idx.find(i) == cp_idx.end())
                new_token(i);
            prod.lhs = cp_idx[i];
            is_non_terminal[cp_idx[i]] = true;

        } else { // 处理产生式右部
            if (cp_idx.find(i) == cp_idx.end())
                new_token(i);
            prod.rhs.push_back(cp_idx[i]);
        }
    }
    productions.push_back(prod); // 新增最后一条产生式
}

RemoveLeftRecursion::Tokens RemoveLeftRecursion::utf8_tokens(const std::string& s) {
    Tokens tokens;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = s[i];
        size_t len = 0;

        if ((c & 0b10000000) == 0)        len = 1; // ASCII
        else if ((c & 0b11100000) == 0b11000000) len = 2;
        else if ((c & 0b11110000) == 0b11100000) len = 3;
        else if ((c & 0b11111000) == 0b11110000) len = 4;
        else {
            throw runtime_error("Invalid UTF-8");
        }
        string token = s.substr(i, len);
        if (token != " ") // 忽略空格
            tokens.push_back(token);
        i += len;
    }
    return tokens;
}

void RemoveLeftRecursion::input(const string &filename){
    ifstream fin(filename);
    if (!fin)
        throw runtime_error("无法打开输入文件！");
    string line;
    while (getline(fin, line)){
        // debug输出读取的每一行
        cout << "读取行: " << line << endl;
        process_line(line);
    }
    
    sort(productions.begin(), productions.end(),
     [&](const Production& a, const Production& b) {
         return idx_cp[a.lhs] < idx_cp[b.lhs];
     });
    // debug输出读取的产生式
    cout << "读取的产生式如下：" << endl;
    for (int i = 0; i < productions.size(); i++){
        cout << idx_cp[productions[i].lhs] << " → ";
        for (auto j: productions[i].rhs)
            cout << idx_cp[j] << " ";
        cout << endl;
    }

    sort(all_tokens.begin(), all_tokens.end());
    // debug输出所有符号
    cout << "所有符号：";
    for (auto i: all_tokens)
        cout << i << " ";
    cout << endl;

    for (auto i: all_tokens)
        if (is_non_terminal[cp_idx[i]])
            non_terminals.push_back(i);
    // debug输出非终结符
    cout << "非终结符：";
    for (auto i: non_terminals)
        cout << i << " ";
    cout << endl;
}

int main(){
    RemoveLeftRecursion rlr;
    rlr.input("./grammar.in");
    return 0;
}
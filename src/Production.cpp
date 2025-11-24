#include "Production.h"

void Productions::init(const Productions &prods){
    all_tokens = prods.all_tokens;
    non_terminals_idx = prods.non_terminals_idx;
    is_non_terminal = prods.is_non_terminal;
    cp_idx = prods.cp_idx;
    idx_cp = prods.idx_cp;
    productions.clear();
}

void Productions::new_token(const string &token){
    all_tokens.push_back(token);
    cp_idx[token] = cp_idx.size();
    idx_cp.push_back(token);
    is_non_terminal.push_back(false);
}

void Productions::set_non_terminal(const string &token){
    // debug
    // cout << "设置非终结符: " << token << endl;
    int idx = get_idx(token);
    if (idx != -1) {
        is_non_terminal[idx] = true;
        non_terminals_idx.push_back(idx);
    }
}

void Productions::push_back(const Production &prod){
    productions.push_back(prod);
}

const Production& Productions::operator[](const int &i){
    return productions[i];
}

int Productions::size(){
    return productions.size();
}

void Productions::clear(){
    productions.clear();
}

Productions::Tokens Productions::utf8_tokens(const std::string& s) {
    Tokens tokens;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = s[i];
        size_t len = 0;

        if ((c & 0b10000000) == 0){ // ASCII
            if (c == '-' && i + 1 < s.size() && s[i + 1] == '>') { // 处理->
                tokens.push_back("→");
                i += 2;
                continue;
            }
            len = 1;
        }
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

void Productions::input(const string &filename){
    ifstream fin(filename);
    if (!fin)
        throw runtime_error("无法打开输入文件！");
    string line;
    new_token("ε"); // 添加空串符号
    while (getline(fin, line)){
        // debug输出读取的每一行
        // cout << "读取行: " << line << endl;
        process_line(line);
    }
    
    std::sort(productions.begin(), productions.end(),
     [&](const Production& a, const Production& b) {
         return idx_cp[a.lhs] < idx_cp[b.lhs];
     });
    // debug输出读取的产生式
    // output_input_productions();

    std::sort(all_tokens.begin(), all_tokens.end());
    // debug输出所有符号
    // cout << "所有符号：";
    // for (auto i: all_tokens)
    //     cout << i << " ";
    // cout << endl;

    // debug输出非终结符
    // cout << "非终结符：";
    // for (auto i: non_terminals)
    //     cout << i << " ";
    // cout << endl;
}

void Productions::process_line(const string &line){
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
            set_non_terminal(i);
            prod.lhs = cp_idx[i];

        } else { // 处理产生式右部
            if (cp_idx.find(i) == cp_idx.end())
                new_token(i);
            prod.rhs.push_back(cp_idx[i]);
        }
    }
    productions.push_back(prod); // 新增最后一条产生式
}

void Productions::output_production(const Production &prod, const string &title){
    if (title.length() > 0)
        cout << title << endl;
    cout << idx_cp[prod.lhs] << " → ";
    for (auto cp: prod.rhs)
        cout << idx_cp[cp];
    cout << '\n' << endl;
}

void Productions::output_productions(const string &title){
    if (title.length() > 0)
        cout << title << endl;
    string last_lhs = "";
    for (auto p: productions){
        if (idx_cp[p.lhs] != last_lhs){ // 新的产生式左部
            if (last_lhs != "")
                cout << endl;
            last_lhs = idx_cp[p.lhs];
            cout << idx_cp[p.lhs] << " → ";
        } else
            cout << " | "; // 同一产生式的其他右部

        for (auto cp: p.rhs) // 输出产生式右部
            cout << idx_cp[cp];
    }
    cout << '\n' << endl;
}

void Productions::sort(){
    std::sort(productions.begin(), productions.end(),
     [&](const Production& a, const Production& b) {
         return idx_cp[a.lhs] < idx_cp[b.lhs];
     });
}

int Productions::get_idx(const string &token){
    return cp_idx.find(token) != cp_idx.end() ? cp_idx[token] : -1;
}

string Productions::get_token(const int &idx){
    return idx_cp[idx];
}

void Productions::output_token_table(){
    cout << "符号表如下：" << endl;
    for (int idx = 0; idx < all_tokens.size(); idx++){
        string token = get_token(idx);
        cout << "   编号 " << idx << " : " << token;
        if (is_non_terminal[idx])
            cout << " (非终结符)";
        cout << endl;
    }
    cout << endl;
}

void Productions::output_non_terminal_table(){
    cout << "非终结符表如下：" << endl;
    for (auto idx: non_terminals_idx)
        cout << "   编号 " << idx << " : " << get_token(idx) << endl;
    cout << endl;
}
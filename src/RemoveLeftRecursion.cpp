#include "RemoveLeftRecursion.h"
    
void RemoveLeftRecursion::RemoveLeftRecursion::process_line(const string &line){
    Tokens tokens = utf8_tokens(line);
    // debug输出分割结果
    // for (auto i: tokens)
    //     cout << "[" << i << "] ";
    // cout << endl;
    bool arrow_found = false;
    int from;
    for (auto i: tokens){
        if (i == "→")
            arrow_found = true;
        else if (i == "|"){
            productions[from].push_back(vector<int>()); // 新增一条产生式
        }
        else if (!arrow_found){ // 处理产生式左部
            if (cp_idx.find(i) == cp_idx.end()){
                cp_idx[i] = cp_idx.size();
                idx_cp[cp_idx[i]] = i;
                productions.push_back(vector<vector<int>>()); // 新增非终结符
            }
            from = cp_idx[i];
            productions[from].push_back(vector<int>());
        } else { // 处理产生式右部
            if (cp_idx.find(i) == cp_idx.end()){
                cp_idx[i] = cp_idx.size();
                idx_cp[cp_idx[i]] = i;
                productions.push_back(vector<vector<int>>()); // 即使是终结符也需要站位
            }
            int to = cp_idx[i];
            productions[from].back().push_back(to); // 此时我们操作的一定是from的最后一条产生式
        }
    }
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
        process_line(line);
    }
    // debug输出读取的产生式
    cout << "读取的产生式如下：" << endl;
    for (int i = 0; i < productions.size(); i++){
        cout << idx_cp[i] << " → ";
        for (int j = 0; j < productions[i].size(); j++){
            for (int k = 0; k < productions[i][j].size(); k++){
                cout << idx_cp[productions[i][j][k]];
            }
            if (j + 1 < productions[i].size())
                cout << " | ";
        }
        cout << endl;
    }
}

int main(){
    RemoveLeftRecursion rlr;
    rlr.input("./grammar.in");
    return 0;
}
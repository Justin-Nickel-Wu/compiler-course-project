#include "RemoveLeftRecursion.h"

void RemoveLeftRecursion::new_token(const string &token){
    all_tokens.push_back(token);
    cp_idx[token] = cp_idx.size();
    idx_cp.push_back(token);
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

void RemoveLeftRecursion::input(const string &filename){
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
    
    sort(productions.begin(), productions.end(),
     [&](const Production& a, const Production& b) {
         return idx_cp[a.lhs] < idx_cp[b.lhs];
     });
    // debug输出读取的产生式
    // output_input_productions();

    sort(all_tokens.begin(), all_tokens.end());
    // debug输出所有符号
    // cout << "所有符号：";
    // for (auto i: all_tokens)
    //     cout << i << " ";
    // cout << endl;

    for (auto i: all_tokens)
        if (is_non_terminal[cp_idx[i]])
            non_terminals.push_back(i);
    // debug输出非终结符
    // cout << "非终结符：";
    // for (auto i: non_terminals)
    //     cout << i << " ";
    // cout << endl;
}

void RemoveLeftRecursion::eliminate_left_recursion(){
    queue<Production> prods_queue;
    queue<Production> new_prods_queue;

    for (int i = 0; i < productions.size(); i++){ // 处理每一条产生式，消除左递归

        prods_queue.push(productions[i]);
        if (i + 1 < productions.size() && productions[i + 1].lhs == productions[i].lhs)
            continue; // 处理完同一非终结符的所有产生式后再进行下一步

        Production prod = productions[i];
        for (auto Aj: new_productions){ // 执行替换
            if (Aj.lhs == prod.lhs)
                break;
            while (!prods_queue.empty()){
                Production result(prod.lhs), front = prods_queue.front();
                prods_queue.pop();
                if (front.rhs[0] == Aj.lhs){ // 需要替换
                    result.rhs.reserve(front.rhs.size() -1 + Aj.rhs.size());
                    if (Aj.rhs.size() != 1 || Aj.rhs[0] != cp_idx["ε"]) // Aj产生式为ε, 则不添加Aj.rhs
                        result.rhs.insert(result.rhs.end(), Aj.rhs.begin(), Aj.rhs.end());
                    result.rhs.insert(result.rhs.end(), front.rhs.begin() + 1, front.rhs.end());
                } else { // 不需要替换
                    result = front;
                }
                new_prods_queue.push(result);
            }
            swap(prods_queue, new_prods_queue);
        }


        bool have_left_recursion = false;
        vector<Production> temp;

        while (!prods_queue.empty()){
            temp.push_back(prods_queue.front());
            prods_queue.pop();
            if (temp.back().lhs == temp.back().rhs[0]) // 判断是否存在左递归
                have_left_recursion = true;
        }
        // debug temp内容
        // cout << "temp内容: " << endl;
        // for (auto p: temp)
        //     output_production(p);

        if (have_left_recursion){ // 如果存在左递归，进行消除
            string token = idx_cp[prod.lhs] + "'";
            new_token(token);
            int new_lhs = cp_idx[token];
            //A  → A a | b | c =>
            //  A  → b A' | c A'
            //  A' → a A' | ε
            for (auto p: temp){
                // debug输出处理过程
                // output_production(p, "处理产生式：");
                // cout << p.lhs << ' ' << p.rhs.size() << endl;
                // cout << p.lhs << ' ' << p.rhs[0] << endl;
                if (p.lhs != p.rhs[0]){
                    Production new_prod(p.lhs, p.rhs.size() + 1);
                    new_prod.rhs.insert(new_prod.rhs.end(), p.rhs.begin(), p.rhs.end());
                    new_prod.rhs.push_back(new_lhs);
                    new_productions.push_back(new_prod);
                } else {
                    Production new_prod(new_lhs, p.rhs.size());
                    new_prod.rhs.insert(new_prod.rhs.end(), p.rhs.begin() + 1, p.rhs.end());
                    new_prod.rhs.push_back(new_lhs);
                    new_productions.push_back(new_prod);
                }
            }
            // 添加ε产生式
            Production eps_prod(new_lhs, 1);
            eps_prod.rhs.push_back(cp_idx["ε"]);
            new_productions.push_back(eps_prod);

        } else { // 不存在左递归，直接加入new_productions
            for (auto p: temp){
                new_productions.push_back(p);
            }
        }
    }

    sort(new_productions.begin(), new_productions.end(), // 新的语法顺序可能被打乱
     [&](const Production& a, const Production& b) {
         return idx_cp[a.lhs] < idx_cp[b.lhs];
     });
}

void RemoveLeftRecursion::output_production(const Production &prod, const string &title){
    if (title.length() > 0)
        cout << title << endl;
    cout << idx_cp[prod.lhs] << " → ";
    for (auto cp: prod.rhs)
        cout << idx_cp[cp];
    cout << endl;
}

void RemoveLeftRecursion::output_productions(const vector<Production> &prods, const string &title){
    if (title.length() > 0)
        cout << title << endl;
    string last_lhs = "";
    for (auto p: prods){
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
    cout << endl;
}

void RemoveLeftRecursion::output_input_productions(){
    output_productions(productions, "输入的产生式如下：");
}

void RemoveLeftRecursion::output_processed_productions(){
    output_productions(new_productions, "消除左递归后的产生式如下：");
}


int main(){
    RemoveLeftRecursion rlr;
    rlr.input("./grammar.in");
    rlr.eliminate_left_recursion();
    rlr.output_processed_productions();
    return 0;
}
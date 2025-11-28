#include "LL1.h"

void LL1::init() {
    prods.output_productions("输入的产生式如下：");
    ff.calculate_first();
    ff.calculate_follow();
    // debug
    ff.output_first();
    ff.output_follow();
}

bool LL1::bulid_parse_table() {
    init();
    /* 构建LL(1)分析表伪代码：
    for (prod: productions)
    begin
        for (symbol: prod.右部)
        begin
            获取FIRST(symbol)
            为分析表[prod.左部, FIRST(symbol) - {ε}]填入prod
            if (FIRST(symbol)不含ε)
                break
        end
        if (prod.右部所有symbol的FIRST均含ε)
            为分析表[prod.左部, FOLLOW(prod.左部)]填入prod
    end
    */

    int epsilon_idx = prods.get_idx("ε");
    for (int i = 0; i < prods.size(); i++) { // 枚举每个产生式
        auto &prod = prods[i];
        bool all_have_epsilon = true; // 标记产生式右部符号是否都能推导出ε
        for (auto symbol_idx: prod.rhs) { // 枚举产生式右部
            auto &first_set = ff.get_First(symbol_idx);
            for (auto idx: first_set) // 枚举该符号的First集
                if (idx != epsilon_idx) {
                    auto key = make_pair(prod.lhs, idx);
                    if (parse_table.count(key) > 0) { // 表格中填入了多项，发生冲突，非LL(1)   
                        cout << "发现冲突的产生式:" << endl;
                        prods.output_production(prods[parse_table[key]]);
                        prods.output_production(prod);
                        return false;
                    } else {
                        parse_table[key] = i;
                    }
                }
            if (!ff.have_epsilon(symbol_idx)){
                all_have_epsilon = false;
                break;
            }
        }

        if (all_have_epsilon) { // 右部符号都能推导出ε，把FOLLOW集加入
            auto &follow_set = ff.get_Follow(prod.lhs);
            for (auto idx: follow_set) {
                auto key = make_pair(prod.lhs, idx);
                if (parse_table.count(key) > 0) { // 表格中填入了多项，发生冲突，非LL(1)   
                    cout << "发现冲突的产生式:" << endl;
                    prods.output_production(prods[parse_table[key]]);
                    prods.output_production(prod);
                    return false;
                } else {
                    parse_table[key] = i;
                }
            }
        }
    }

    return true;
}

void LL1::output_parse_table(const string &filename) {
    ofstream out_md("./output/" + filename +".md");

    vector<int> row, col;
    for (auto i = prods.non_terminal_begin(); i != prods.non_terminal_end(); ++i) {
        row.push_back(*i);
    }
    for (auto i = prods.token_begin(); i != prods.token_end(); ++i)
        if (*i != "ε" && !prods.is_non_terminal(prods.get_idx(*i)))
            col.push_back(prods.get_idx(*i));
    
    // 输出表头
    out_md << "| 非终结符 |";
    for (auto c: col) {
        out_md << " " << prods.get_token(c) << " |";
    }
    out_md << "\n";
    out_md << "|";
    for (int i = 1; i <= col.size() + 1; i++)
        out_md << ":-:|";
    out_md << "\n";

    // 输出表格内容
    for (auto i: row) {
        out_md << "| " << prods.get_token(i) << " |";
        for (auto j: col) {
            auto key = make_pair(i, j);
            if (parse_table.count(key) > 0) {
                out_md << " ";
                prods.output_production_for_table(out_md, prods[parse_table[key]]);
                out_md << " |";
            } else {
                out_md << "   |";
            }
        }
        out_md << "\n";
    }
    out_md << "\n";

    // 表格格式化内容
    double width_percent = 100.0 / (col.size() + 1);
    string s = to_string((int)width_percent) + "%";
    out_md << "<style>\n";
    out_md << "table th:first-of-type {\n    width: " << s << ";\n}\n";
    for (int i = 2; i <= col.size() + 1; i++) {
        out_md << "table th:nth-of-type(" << i << ") {\n    width: " << s << ";\n}\n";
    }
}
bool LL1::match(const string &input_str) {
    Productions temp; // 临时存储输入的产生式,借用Productions类方便输入
    temp.process_line("@ -> " + input_str + " $"); // 假定开始符号为S，输入结束符号为$。@->用于占位。
    Production input_prod = temp[0];

    // debug
    // temp.output_productions("待匹配的输入字符串产生式如下：");

    /*LL(1)匹配伪代码：
    初始化栈，栈底放入输入结束符号$，栈顶放入文法开始符号
    for (ch: 输入字符串)
    begin
        if (栈顶是终结符)
            if (栈顶与ch相同)
                弹出栈顶，继续下一个输入符号
            else
                报错，匹配失败
        
        if (栈空)
            报错，匹配失败
        
        在分析表中查找[栈顶, ch]对应的产生式
        if (找不到对应产生式)
            报错，匹配失败
        弹出栈顶

        将产生式右部符号逆序入栈(遇到ε不入栈)
    end
    */

    stack<int> sta;
    sta.push(prods.get_idx("$"));
    sta.push(prods.get_start_idx());

    int i = 0;
    int finish_idx = prods.get_idx("$");
    while (i < input_prod.rhs.size()) {
        if (!sta.empty() && sta.top() != finish_idx && !prods.is_non_terminal(sta.top())) { // 如果栈顶是终结符，弹出，输入指针后移
            if (prods.get_token(sta.top()) != temp.get_token(input_prod.rhs[i])) {
                return Err(temp, i, "匹配失败：终结符不匹配");
            }
            sta.pop(), i++;
            continue;
        }

        if (sta.empty()) { // 栈空但输入未读完，匹配失败
            return Err(temp, i, "匹配失败：栈空但输入未读完");
        }

        int input_idx = prods.get_idx(temp.get_token(input_prod.rhs[i]));
        string input_token = prods.get_token(input_idx);
        auto key = make_pair(sta.top(), input_idx);

        //debug
        // cout << "栈大小: " << sta.size() << ", 栈顶符号: " << prods.get_token(sta.top()) << ", 当前输入符号: " << input_token << endl;

        if (input_token == "$" && sta.top() == input_idx) { // "$$"时匹配成功
            cout << "匹配成功!\n" << endl;
            return true;
        }

        if (parse_table.count(key) == 0) { // 表格中无对应产生式，匹配失败
            return Err(temp, i, "匹配失败：在解析表中找不到对应产生式");
        }

        int prod_idx = parse_table[key];
        sta.pop();
        Production push_prod = prods[prod_idx];
        for (int j = push_prod.rhs.size() - 1; j >= 0; j--) {

            if (prods.get_token(push_prod.rhs[j]) != "ε") // 遇到ε不入栈
                sta.push(push_prod.rhs[j]);
        }
    }

    return Err(temp, input_prod.rhs.size() - 1, "匹配失败：输入未读完但栈空");
}

bool LL1::Err(Productions &prods, int pos, const string &msg) {
    cout << msg << endl;
    Production prod = prods[0];
    for (int i = 0; i < prod.rhs.size() - 1; i++) {
        if (i == pos)
            cout << " >> " << prods.get_token(prod.rhs[i]) << " << ";
        else
            cout << prods.get_token(prod.rhs[i]);
    }
    if (pos >= prod.rhs.size() - 1)
        cout << "「$」";
    cout << '\n' << endl;
    return false;
}
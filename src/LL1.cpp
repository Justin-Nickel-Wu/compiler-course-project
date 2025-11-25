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
                        cout << "发现冲突的产生式: ";
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
                    cout << "发现冲突的产生式: ";
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
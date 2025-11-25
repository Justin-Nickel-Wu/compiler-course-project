#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <bits/stdc++.h>
using namespace std;

struct Production {
    int lhs; // 产生式左部非终结符编号
    vector<int> rhs; // 产生式右部符号编号序列

    Production(int lhs = -1, int reserve_len = 0): lhs(lhs) { 
        rhs.reserve(reserve_len);
    }
};

class Productions {
private:
    typedef vector<string> Tokens;
    Tokens all_tokens;                  // 存储所有输入符号
    vector<int> non_terminals_idx;      // 存储非终结符
    vector<bool> non_terminal_flag;     // 标记某个符号是否为非终结符
    map<string,int> cp_idx;             // 将输入符号与内部编号相对应
    vector<string> idx_cp;              // 将内部编号与输入符号相对应
    vector<Production> productions;     // 存储所有产生式
    int start = -1;                     // 文法开始符号编号(约定第一个输入的产生式左部为开始符号,-1为未设置)

public:
    void init(const Productions &prods);        // 初始化为另一个Productions的内容.会保留符号表，但清空产生式列表
    void new_token(const string &token);        // 新增一个符号
    void set_non_terminal(const string &token); // 新增一个非终结符
    Tokens utf8_tokens(const std::string& s);   // 将输入字符串按utf8编码分割为符号序列
    void process_line(const string &line);      // 处理输入的一行产生式
    void input(const string &filename);         // 从文件中读取产生式
    void clear();                               // 清空产生式列表,但保留符号表

    void push_back(const Production &prod);     // 添加产生式
    void sort();                                // 对产生式按左部非终结符编号进行排序
    int get_idx(const string &token);           // 获取符号的内部编号,不存在返回-1
    string get_token(const int &idx);           // 获取符号的字符串表示
    int size();                                 // 获取产生式数量
    int token_size();                           // 获取符号数量
    int non_terminal_size();                    // 获取非终结符数量
    const Production& operator[](const int &i); // 重载下标运算符
    bool is_non_terminal(const int &idx);       // 判断某个符号是否为非终结符
    int get_start_idx();                        // 获取文法开始符号编号

    auto begin(){ return productions.begin(); }                    // 获取产生式开始迭代器
    auto end(){ return productions.end(); }                        // 获取产生式结束迭代器
    auto token_begin(){ return all_tokens.begin(); }               // 获取符号开始迭代器
    auto token_end(){ return all_tokens.end(); }                   // 获取符号结束迭代器
    auto non_terminal_begin(){ return non_terminals_idx.begin(); } // 获取非终结符开始迭代器
    auto non_terminal_end(){ return non_terminals_idx.end(); }     // 获取非终结符结束迭代器

    void output_production(const Production &prod, const string &title = ""); // 输出单条产生式
    void output_productions(const string &title = "");                        // 输出所有产生式
    void output_token_table();                                                // 输出符号表
    void output_non_terminal_table();                                         // 输出非终结符表
    void output_production_for_table(ofstream &out_md, const Production &prod); // 输出单条产生式，用于表格
};

#endif
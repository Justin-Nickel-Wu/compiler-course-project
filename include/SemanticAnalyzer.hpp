#ifndef SemanticAnalyzer_hpp
#define SemanticAnalyzer_hpp

#include <bits/stdc++.h>
using namespace std;

#include "parse_tree.hpp"

// 标识符的类型，包含变量、符号、数组
enum SymbolType {
    VAR_SYMBOL,  // 变量符号
    FUNC_SYMBOL, // 函数符号
    ARRAY_SYMBOL // 数组符号
};

struct SymbolInfo {
    // 类型，使用词法token
    int type;

    SymbolType symbol_type;

    SymbolInfo() : type(-1), symbol_type(VAR_SYMBOL) {}
    SymbolInfo(int type, SymbolType symbol_type) : type(type), symbol_type(symbol_type) {}
};

class SemanticAnalyzer {
private:
    ParseTree &AST;

    // 各种节点hook
    // 为了处理嵌套逻辑，使用类似mut方法
    int IN_LOOP, IN_FUNC_DEF, IN_FUNC_IDENT_DEF, IN_FUNC_FPARAMS_DEF;

    bool SOMETHING_WRONG; // 语义分析过程中是否出错
    int GLOBAL_VAR_TYPE;  // 当前处理的变量类型，有很多情况会使用到该变量，正确性由使用者自身保证

    vector<pair<int, string>> func_fparams_stack;          // 函数形参栈
    vector<unordered_map<string, SymbolInfo>> scope_stack; // 符号表栈

    struct ASTInfoNode {
        int type;               // 数值的类型，使用词法token
        SymbolType symbol_type; // 标识符的类型，使用enum SymbolType
        vector<int> dims;       // 数组维度
    };

    vector<ASTInfoNode> ASTInfo; // AST辅助信息

    void push_scope(); // 创建作用域
    void pop_scope();  // 销毁作用域

    bool declare_var(int type, const string &ident);         // 声明变量，失败返回false（重复定义）
    bool declare_func(int return_type, const string &ident); // 声明函数，失败返回false（重复定义）
    SymbolInfo find(const string &ident);                    // 根据标识符查找信息

    void checkLVal(int node_id);          // 处理LVal节点（变量使用）
    void checkBreakContinue(int node_id); // 处理break和continue语句
    void checkFuncCall(int node_id);      // 处理函数调用
    void declareFunction(int node_id);    // 处理函数声明
    void declareVariable(int node_id);    // 处理变量声明
    void enterNode(int node_id);          // 进入节点时的处理
    void leaveNode(int node_id);          // 离开节点时的处理

    void init();                    // 初始化
    void SemanticAnalyzeDFS(int p); // 语义分析递归函数

public:
    SemanticAnalyzer(ParseTree &ast) : AST(ast) {}
    bool SemanticAnalyze(); // 进行语义分析
};

#endif
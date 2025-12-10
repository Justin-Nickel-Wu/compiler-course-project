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
    int                    type; // 数值类型，使用词法token，-1表示不存在
    SymbolType             symbol_type;
    int                    dims;        // 数组维度，0表示非数组，>0表示数组维度
    vector<pair<int, int>> func_params; // 函数参数列表，存储每个参数的类型和维度

    SymbolInfo() : type(-1), symbol_type(VAR_SYMBOL) {}
    SymbolInfo(int type, SymbolType symbol_type, int dims) : type(type), symbol_type(symbol_type), dims(dims) {}
};

class SemanticAnalyzer {
private:
    ofstream var_list_out{"./output/var_list.txt"};

    ParseTree &AST;

    // 各种节点hook
    // 为了处理嵌套逻辑，使用类似mut方法
    int IN_LOOP, IN_FUNC_DEF, IN_FUNC_IDENT_DEF, IN_FUNC_FPARAMS_DEF;

    bool   SOMETHING_WRONG;         // 语义分析过程中是否出错
    int    GLOBAL_VAR_TYPE;         // 用于缓存当前声明的函数或是变量的类型。有很多情况会使用到该变量，即用即弃，正确性由使用者自身保证
    int    GLOBAL_FUNC_RETURN_TYPE; // 用于缓存当前函数的返回值类型
    string GLOBAL_FUNC_IDENT;       // 用于缓存当前函数的标识符
    struct fparamsInfo {
        int    type, dims;
        string ident;
        fparamsInfo(int t, const string &i, int d) : type(t), dims(d), ident(i) {}
    };
    vector<fparamsInfo>                       func_fparams_stack; // 函数形参栈
    vector<unordered_map<string, SymbolInfo>> scope_stack;        // 符号表栈

    struct ASTInfoNode {
        int        type;        // 数值的类型，使用词法token -1:不需要类型信息 -2:类型错误
        SymbolType symbol_type; // 标识符的类型，使用enum SymbolType
        int        dims;        // 数组维度，-1表示维度错误，0表示非数组

        ASTInfoNode() : type(-1) {}
    };

    vector<ASTInfoNode> ASTInfo; // AST辅助信息

    void push_scope(); // 创建作用域
    void pop_scope();  // 销毁作用域

    bool declare_var(int type, const string &ident, int dims); // 声明变量，失败返回false（重复定义）
    bool declare_func(int return_type, const string &ident);   // 声明函数，失败返回false（重复定义）

    SymbolInfo find(const string &ident); // 根据标识符查找信息

    bool checkLVal(int node_id);          // 处理LVal节点（变量使用）
    bool checkBreakContinue(int node_id); // 处理break和continue语句
    int  checkFuncCall(int node_id);      // 处理函数调用，若有效则返回函数返回值类型的token，否则返回0
    bool checkExp(int node_id);           // 处理表达式节点
    bool checkReturn(int node_id);        // 处理return语句节点
    bool checkAssign(int node_id);        // 处理赋值
    bool checkInitVal(int node_id);       // 处理初始化值
    bool checkVarDimList(int node_id);    // 处理数组变量定义时的维度
    bool checkFuncFParam(int node_id);    // 处理函数形参
    void declareFunction(int node_id);    // 处理函数声明
    void declareVariable(int node_id);    // 处理变量声明
    void enterNode(int node_id);          // 进入节点时的处理
    void leaveNode(int node_id);          // 离开节点时的处理

    void init();                    // 初始化
    void SemanticAnalyzeDFS(int p); // 语义分析递归函数

public:
    bool OUTPUT_VAR_LIST = false; // 是否输出变量列表，DEBUG用
    SemanticAnalyzer(ParseTree &ast) : AST(ast) {}
    bool SemanticAnalyze(); // 进行语义分析
    void printASTInfo();    // DEBUG输出AST辅助信息
};

// 判断value是否在list中
template <typename T, typename U>
bool in(const T &value, std::initializer_list<U> list) {
    return std::find(list.begin(), list.end(), value) != list.end();
}

#endif
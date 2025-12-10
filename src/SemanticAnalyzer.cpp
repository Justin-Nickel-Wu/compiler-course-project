#include "SemanticAnalyzer.hpp"
#include "parse_tree.hpp"
#include "error_handler.hpp"
#include "bison.hpp"

#include <bits/stdc++.h>
using namespace std;

void SemanticAnalyzer::push_scope() {
    scope_stack.push_back(unordered_map<string, SymbolInfo>());
}

void SemanticAnalyzer::pop_scope() {
    if (scope_stack.empty()) {
        cerr << "Error: Attempt to pop from an empty scope stack." << endl;
        exit(1);
    }
    scope_stack.pop_back();
}

bool SemanticAnalyzer::declare_var(int type, const string &ident) {
    if (scope_stack.empty()) {
        cerr << "Error: No scope available to declare symbol." << endl;
        exit(1);
    }
    auto &current = scope_stack.back();

    // 如果变量重复定义
    if (current.find(ident) != current.end()) {
        return false;
    }

    SymbolInfo info(type, VAR_SYMBOL);
    current[ident] = info;

    return true;
}

SymbolInfo SemanticAnalyzer::find(const string &ident) {
    SymbolInfo ret;
    for (auto it : scope_stack) {
        if (it.find(ident) != it.end()) {
            return it[ident];
        }
    }
    return ret;
}

bool SemanticAnalyzer::declare_func(int return_type, const string &ident) {
    // 函数声明时一定处于全局作用域中
    auto      &current = scope_stack.front();
    SymbolInfo info(return_type, FUNC_SYMBOL);
    if (current.find(ident) != current.end()) {
        return false;
    }
    current[ident] = info;
    return true;
}

void SemanticAnalyzer::init() {
    SOMETHING_WRONG     = false;
    IN_LOOP             = 0;
    IN_FUNC_DEF         = 0;
    IN_FUNC_IDENT_DEF   = 0;
    IN_FUNC_FPARAMS_DEF = 0;
    GLOBAL_VAR_TYPE     = -1;
    scope_stack.clear();

    ASTInfo.clear();
    ASTInfo.resize(AST.nodes.size());
}

bool SemanticAnalyzer::SemanticAnalyze() {
    init();
    push_scope(); // 全局作用域。

    SemanticAnalyzeDFS(AST.get_root());

    // 最后销毁全局作用域
    pop_scope();

    return !SOMETHING_WRONG;
}

void SemanticAnalyzer::SemanticAnalyzeDFS(int p) {
    ParseTreeNode &node = AST.nodes[p];

    /*==状态处理==*/
    enterNode(p);

    /*==语义提取==*/
    declareVariable(p);    // 处理变量声明
    declareFunction(p);    // 处理函数声明
    checkBreakContinue(p); // 处理break和continue语句

    /*==递归遍历==*/
    for (int son_id : node.son) {
        SemanticAnalyzeDFS(son_id);
    }

    /*==维护类型==*/
    checkLVal(p); // 处理变量使用 (Val)
    checkEXP(p);  // 处理表达式节点

    /*==状态回收==*/
    leaveNode(p);
}

void SemanticAnalyzer::declareFunction(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    // 处理函数声明时的标识符
    if (IN_FUNC_IDENT_DEF && node.token_type == IDENT) {
        if (!declare_func(GLOBAL_VAR_TYPE, node.ident)) {
            SOMETHING_WRONG = true;
            Err('4', node.line, "Redefinition of function \"" + string(node.ident) + "\".");
        }
    }

    // 处理函数声明时的形参类型
    if (IN_FUNC_FPARAMS_DEF && node.name == "Btype") {
        GLOBAL_VAR_TYPE = AST.nodes[node.son[0]].token_type;
    }

    // 处理函数声明时的形参标识符
    if (IN_FUNC_FPARAMS_DEF && node.token_type == IDENT) {
        func_fparams_stack.push_back({GLOBAL_VAR_TYPE, string(node.ident)});
    }

    // 处理函数定义时引入的形参
    if (IN_FUNC_DEF && node.name == "Block") {
        for (auto &param : func_fparams_stack) {
            if (!declare_var(param.first, param.second)) {
                SOMETHING_WRONG = true;
                Err('2', node.line, "Redefinition of variable \"" + param.second + "\".");
            }
        }
    }
}

void SemanticAnalyzer::declareVariable(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    // 处理声明变量时的类型
    if (node.name == "BType") {
        int q           = node.son[0]; // 获取类型节点
        GLOBAL_VAR_TYPE = AST.nodes[q].token_type;
    }

    // 处理变量声明时的标识符
    if (node.name == "VarDef") {
        int    q     = node.son[0]; // 获取标识符节点
        string ident = AST.nodes[q].ident;
        if (!declare_var(GLOBAL_VAR_TYPE, ident)) {
            SOMETHING_WRONG = true;
            Err('2', node.line, "Redefinition of variable \"" + ident + "\".");
        }
    }
}

bool SemanticAnalyzer::checkLVal(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    if (node.name == "LVal") {
        string     ident = AST.nodes[node.son[0]].ident;
        SymbolInfo info  = find(ident);
        // 使用未声明的变量
        if (info.type == -1) {
            SOMETHING_WRONG = true;
            Err('1', node.line, "Use of undeclared variable \"" + ident + "\".");
            return 0;
        }
        // 将函数当作变量使用
        else if (info.symbol_type == FUNC_SYMBOL) {
            SOMETHING_WRONG = true;
            Err('6', node.line, "\"" + ident + "\" is a function, not a variable.");
            return 0;
        }
        // 正常
        else {
            ASTInfo[node_id].type        = info.type;
            ASTInfo[node_id].symbol_type = info.symbol_type;
        }
    }
    return 1;
}

bool SemanticAnalyzer::checkBreakContinue(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    // 处理break和continue语句
    if (!IN_LOOP && (node.token_type == BREAK || node.token_type == CONTINUE)) {
        SOMETHING_WRONG = true;
        Err('3', node.line, "\"break\" or \"continue\" statement not within a loop.");
        return 0;
    }
    return 1;
}

int SemanticAnalyzer::checkFuncCall(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    // 处理函数调用
    if (node.name == "UnaryExp") {
        ParseTreeNode son_node = AST.nodes[node.son[0]];
        if (son_node.token_type == IDENT) {
            SymbolInfo info = find(son_node.ident);
            // 调用未声明的函数
            if (info.type == -1) {
                SOMETHING_WRONG = true;
                Err('3', node.line, "Call to undeclared function \"" + string(son_node.ident) + "\".");
                return 0;
            }
            // 将变量当作函数使用
            else if (info.symbol_type != FUNC_SYMBOL) {
                SOMETHING_WRONG = true;
                Err('5', node.line, "\"" + string(son_node.ident) + "\" is a variable, not a function.");
                return 0;
            }
        }
    }
    return 1;
}

bool SemanticAnalyzer::checkEXP(int node_id) {
    // 把所有有类型的儿子节点取出，有float则当前节点就是float，否则是int。
    // 某些节点需要特殊处理
    const ParseTreeNode &node = AST.nodes[node_id];

    // 特殊处理：如果是函数调用
    if (node.name == "UnaryExp" && AST.nodes[node.son[0]].token_type == IDENT) {
        int ret_type = checkFuncCall(node_id);
        if (ret_type != -1) {
            ASTInfo[node_id].type        = ret_type;
            ASTInfo[node_id].symbol_type = FUNC_SYMBOL;
            return 1;
        } else {
            ASTInfo[node_id].type        = -2;
            ASTInfo[node_id].symbol_type = FUNC_SYMBOL;
            return 0;
        }
    }

    // 特殊情况： 常量
    if (node.name == "Number") {
        ParseTreeNode son_node = AST.nodes[node.son[0]];
        if (son_node.token_type == INT_CONST) {
            ASTInfo[node_id].type        = INT;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
        } else if (son_node.token_type == FLOAT_CONST) {
            ASTInfo[node_id].type        = FLOAT;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
        }
        return 1;
    }

    // 特殊情况： MulExp涉及取模时需要均是int
    if (node.name == "MulExp" && node.son.size() == 3 && AST.nodes[node.son[1]].token_type == MOD) {
        int left_type  = ASTInfo[node.son[0]].type;
        int right_type = ASTInfo[node.son[2]].type;

        // 避免重复报错
        if (left_type == -2 || right_type == -2) {
            ASTInfo[node_id].type        = -2;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
            return 0;
        }

        if (left_type != INT || right_type != INT) {
            SOMETHING_WRONG = true;
            Err("11", node.line, "Modulus operator \"%\" requires integer operands.");
            ASTInfo[node_id].type        = -2;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
            return 0;
        } else {
            ASTInfo[node_id].type        = INT;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
            return 1;
        }
    }

    // 一般情况：表达式节点
    if (in(node.name, {"Exp", "AddExp", "MulExp", "UnaryExp", "PrimaryExp"})) {
        bool has_float = false;
        for (int son_id : node.son)
            // 如果子节点已经发生了类型错误，直接返回，避免重复报错
            if (ASTInfo[son_id].type == -2) {
                return 0;
            } else if (ASTInfo[son_id].type == FLOAT)
                has_float = true;

        if (has_float) {
            ASTInfo[node_id].type        = FLOAT;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
        } else {
            ASTInfo[node_id].type        = INT;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
        }
    }
    return 1;
}

void SemanticAnalyzer::enterNode(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    // 进入新作用域
    if (node.name == "Block") {
        push_scope();
    }

    // 进入循环体
    if (node.name == "Stmt" && AST.nodes[node.son[0]].token_type == WHILE) {
        ++IN_LOOP;
    }

    // 进入函数定义
    if (node.name == "FuncDef") {
        ++IN_FUNC_DEF;
    }

    // 进入函数标识符定义
    if (node.name == "FuncType") {
        GLOBAL_VAR_TYPE = AST.nodes[node.son[0]].token_type;
        ++IN_FUNC_IDENT_DEF;
    }

    // 进入函数形参定义
    if (node.name == "FuncFParams") {
        if (IN_FUNC_FPARAMS_DEF == 0) // 第一次使用确保清空，可能不必要
            func_fparams_stack.clear();
        ++IN_FUNC_FPARAMS_DEF;
    }
}

void SemanticAnalyzer::leaveNode(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    // 离开作用域
    if (node.name == "Block") {
        pop_scope();
    }

    // 离开循环体
    if (node.name == "Stmt" && AST.nodes[node.son[0]].token_type == WHILE) {
        --IN_LOOP;
    }

    // 离开函数定义 (函数定义后的第一个block即退出，仅用于引入形参)
    if (IN_FUNC_DEF && node.name == "Block") {
        --IN_FUNC_DEF;
    }

    // 离开函数标识符定义
    if (IN_FUNC_IDENT_DEF && node.token_type == LPARENT) {
        --IN_FUNC_IDENT_DEF;
    }

    // 离开函数形参定义
    if (node.name == "FuncFParams") {
        --IN_FUNC_FPARAMS_DEF;
    }
}
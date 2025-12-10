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

bool SemanticAnalyzer::declare_var(int type, const string &ident, int dims) {
    if (scope_stack.empty()) {
        cerr << "Error: No scope available to declare symbol." << endl;
        exit(1);
    }
    auto &current = scope_stack.back();

    // 如果变量重复定义
    if (current.find(ident) != current.end()) {
        return false;
    }

    SymbolInfo info(type, VAR_SYMBOL, dims);
    current[ident] = info;

    if (OUTPUT_VAR_LIST) {
        var_list_out << "Declared variable: " << ident << ", type: " << type << ", dims: " << dims << endl;
    }

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
    SymbolInfo info(return_type, FUNC_SYMBOL, 0);
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
    declareFunction(p); // 处理函数声明

    /*==递归遍历==*/
    for (int son_id : node.son) {
        SemanticAnalyzeDFS(son_id);
    }

    /*==维护类型==*/
    declareVariable(p);    // 处理变量声明
    checkBreakContinue(p); // 处理break和continue语句
    checkLVal(p);          // 处理变量使用 (Val)
    checkExp(p);           // 处理表达式节点
    checkReturn(p);        // 处理return语句节点
    checkAssign(p);        // 处理赋值
    checkInitVal(p);       // 处理初始化值
    checkVarDimList(p);    // 处理数组变量定义时的维度
    checkFuncFParam(p);    // 处理函数形参

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

    // 处理函数定义时引入的形参
    if (IN_FUNC_DEF && node.name == "Block") {
        for (auto &param : func_fparams_stack) {
            if (!declare_var(param.type, param.ident, param.dims)) {
                SOMETHING_WRONG = true;
                Err('2', node.line, "Redefinition of variable \"" + param.ident + "\".");
            }
        }
    }
}

void SemanticAnalyzer::declareVariable(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    // 处理声明变量时的类型
    if (node.name == "BType") {        // 只有定义函数或者变量时会出现BType，且两者不会冲突
        int q           = node.son[0]; // 获取类型节点
        GLOBAL_VAR_TYPE = AST.nodes[q].token_type;
    }

    // 处理变量声明时的标识符
    if (in(node.name, {"VarDef", "ConstDef"})) { // TODO: 目前无法区分常量和变量
        int    q     = node.son[0];              // 获取标识符节点
        int    dims  = 0;
        string ident = AST.nodes[q].ident;

        if (in(node.son.size(), {2, 4}))
            dims = ASTInfo[node.son[1]].dims;

        if (!declare_var(GLOBAL_VAR_TYPE, ident, dims)) {
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
        int        dims  = node.son.size() == 2 ? ASTInfo[node.son[1]].dims : 0;
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
        // 数组维度过多
        else if (info.dims < dims) {
            SOMETHING_WRONG = true;
            Err('7', node.line, "Array \"" + ident + "\" don't have that many dimensions.");
            ASTInfo[node_id].type        = -2;
            ASTInfo[node_id].symbol_type = info.symbol_type;
            return 0;
        }
        // 正常
        else {
            ASTInfo[node_id].type        = info.type;
            ASTInfo[node_id].symbol_type = info.symbol_type;
            ASTInfo[node_id].dims        = info.dims - dims;
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
    ParseTreeNode son_node = AST.nodes[node.son[0]];
    SymbolInfo    info     = find(son_node.ident);

    // 调用未声明的函数
    if (info.type == -1) {
        SOMETHING_WRONG = true;
        Err('3', node.line, "Call to undeclared function \"" + string(son_node.ident) + "\".");
        return 0;
    }

    // 将变量当作函数使用
    if (info.symbol_type != FUNC_SYMBOL) {
        SOMETHING_WRONG = true;
        Err('5', node.line, "\"" + string(son_node.ident) + "\" is a variable, not a function.");
        return 0;
    }
    return info.type;
}

bool SemanticAnalyzer::checkExp(int node_id) {
    // 把所有有类型的儿子节点取出，有float则当前节点就是float，否则是int。
    // 某些节点需要特殊处理
    const ParseTreeNode &node = AST.nodes[node_id];

    // 特殊处理：如果是函数调用
    if (node.name == "UnaryExp" && AST.nodes[node.son[0]].token_type == IDENT) {
        int ret_type = checkFuncCall(node_id);
        // 有效结果
        if (ret_type == INT || ret_type == FLOAT) {
            ASTInfo[node_id].type        = ret_type;
            ASTInfo[node_id].symbol_type = FUNC_SYMBOL;
            return 1;
        }
        // void
        else if (ret_type == VOID) {
            SOMETHING_WRONG = true;
            Err("11", node.line, "Function \"" + string(AST.nodes[node.son[0]].ident) + "()\" does not return a value.");
            ASTInfo[node_id].type        = -2;
            ASTInfo[node_id].symbol_type = FUNC_SYMBOL;
            return 0;
        }
        // 无结果
        else {
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
            ASTInfo[node_id].dims        = 0;
        } else if (son_node.token_type == FLOAT_CONST) {
            ASTInfo[node_id].type        = FLOAT;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
            ASTInfo[node_id].dims        = 0;
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
    if (in(node.name, {"Exp", "ConstExp", "AddExp", "MulExp", "UnaryExp", "PrimaryExp"})) {
        bool has_float = false, has_array = false;
        int  dims;
        for (int son_id : node.son)
            // 如果子节点已经发生了类型错误，直接返回，避免重复报错
            if (ASTInfo[son_id].type == -2) {
                ASTInfo[node_id].type        = -2;
                ASTInfo[node_id].symbol_type = VAR_SYMBOL;
                return 0;
            } else {
                if (ASTInfo[son_id].type == FLOAT)
                    has_float = true;
                if (ASTInfo[son_id].type == INT || ASTInfo[son_id].type == FLOAT) {
                    dims = ASTInfo[son_id].dims;
                    if (ASTInfo[son_id].dims > 0)
                        has_array = true;
                }
            }

        // 只有直接继承数组的节点才是数组类型，其他情况一定引入了运算，报错
        if (has_array && node.son.size() > 1) {
            SOMETHING_WRONG = true;
            Err("11", node.line, "Array type cannot participate in arithmetic operations.");
            ASTInfo[node_id].type        = -2;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
            return 0;
        }

        if (has_float) {
            ASTInfo[node_id].type        = FLOAT;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
            ASTInfo[node_id].dims        = dims;
        } else {
            ASTInfo[node_id].type        = INT;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
            ASTInfo[node_id].dims        = dims;
        }
    }
    return 1;
}

bool SemanticAnalyzer::checkReturn(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    if (node.name == "Stmt" && AST.nodes[node.son[0]].token_type == RETURN) {
        if (node.son.size() == 2) {
            if (GLOBAL_FUNC_RETURN_TYPE != VOID) {
                SOMETHING_WRONG = true;
                Err("10", node.line, "Return a value in a void function.");
                return 0;
            }
        } else {
            int Exp_type = ASTInfo[node.son[1]].type;

            if (Exp_type == -2) return 0; // 避免重复报错

            if (GLOBAL_FUNC_RETURN_TYPE == VOID) {
                SOMETHING_WRONG = true;
                Err("10", node.line, "Return nothing in a non-void function.");
                return 0;
            }

            if (Exp_type == FLOAT && GLOBAL_FUNC_RETURN_TYPE == INT) {
                SOMETHING_WRONG = true;
                Err("10", node.line, "Return float in an int function.");
                return 0;
            }
        }
    }
    return 1;
}

bool SemanticAnalyzer::checkAssign(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    // 判断赋值语句两侧类型是否匹配
    if (node.name == "Stmt" && node.son.size() == 4 && AST.nodes[node.son[1]].token_type == ASSIGN) {
        int left_type  = ASTInfo[node.son[0]].type;
        int left_dims  = ASTInfo[node.son[0]].dims;
        int right_type = ASTInfo[node.son[2]].type;
        int right_dims = ASTInfo[node.son[2]].dims;

        if (left_type == -2 || right_type == -2) return 0; // 避免重复报错

        if (left_dims > 0 || right_dims > 0) {
            SOMETHING_WRONG = true;
            Err("11", node.line, "Cannot assign array types.");
            return 0;
        }

        if (left_type == INT && right_type == FLOAT) {
            SOMETHING_WRONG = true;
            Err("11", node.line, "Cannot assign float value to int variable.");
            return 0;
        }
    }

    // 判断变量初始化时的两侧类型是否匹配
    if (in(node.name, {"VarDef", "ConstDef"}) && in(node.son.size(), {3, 4})) {
        int left_type  = GLOBAL_VAR_TYPE; // 此时GLOBAL_VAR_TYPE变量一定还是有效的
        int right_type = ASTInfo[node.son[node.son.size() - 1]].type;

        if (right_type == -2) return 0; // 避免重复报错

        if (left_type == INT && right_type == FLOAT) {
            SOMETHING_WRONG = true;
            Err("11", node.line, "Cannot assign float value to int variable.");
            return 0;
        }
    }

    return 1;
}

bool SemanticAnalyzer::checkInitVal(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    // 处理初始化值
    // TODO: 识别是不是数组
    if (in(node.name, {"InitVal", "ConstInitVal"})) {
        if (node.son.size() == 1) {
            ASTInfo[node_id].type        = ASTInfo[node.son[0]].type;
            ASTInfo[node_id].symbol_type = VAR_SYMBOL;
        }
    }

    return 1;
}

bool SemanticAnalyzer::checkVarDimList(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];

    if (in(AST.nodes[node_id].name, {"VarDimList", "ConstDimList", "LValDimsList", "FuncFParamDimsList"})) {
        // 计算维数
        if (AST.nodes[node.son[0]].token_type == LBRACK) {
            ASTInfo[node_id].dims = 1;
        } else {
            if (ASTInfo[node.son[0]].dims == -1) { // 子节点维度错误，避免重复报错
                ASTInfo[node_id].dims = -1;
                return 0;
            }
            ASTInfo[node_id].dims = ASTInfo[node.son[0]].dims + 1;
        }

        int Exp_type = ASTInfo[node.son[AST.nodes[node.son[0]].token_type == LBRACK ? 1 : 2]].type;
        if (Exp_type == -2) return 0; // 避免重复报错

        if (Exp_type != INT) {
            ASTInfo[node_id].dims = -1;
            SOMETHING_WRONG       = true;
            Err('7', node.line, "Array size must be an integer.");
            return 0;
        }
    }
    return 1;
}

bool SemanticAnalyzer::checkFuncFParam(int node_id) {
    const ParseTreeNode &node = AST.nodes[node_id];
    if (node.name == "FuncFParam") {
        const string &ident = AST.nodes[node.son[1]].ident;
        int           dims  = 0;

        if (node.son.size() == 4)
            dims = 1;
        if (node.son.size() == 5)
            dims = ASTInfo[node.son[4]].dims + 1;
        func_fparams_stack.push_back(fparamsInfo(GLOBAL_VAR_TYPE, ident, dims));
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
        GLOBAL_VAR_TYPE         = AST.nodes[node.son[0]].token_type;
        GLOBAL_FUNC_RETURN_TYPE = GLOBAL_VAR_TYPE;
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

void SemanticAnalyzer::printASTInfo() {
    ofstream out("./output/ast_info_debug.txt");
    for (int i = 0; i < AST.nodes.size(); ++i) {
        const ParseTreeNode &node = AST.nodes[i];
        const ASTInfoNode   &info = ASTInfo[i];
        out << "Node ID: " << i << ", Name: " << node.name << ", Type: " << info.type
            << ", Symbol Type: " << info.symbol_type << ", Dims: " << info.dims << endl;
    }
}
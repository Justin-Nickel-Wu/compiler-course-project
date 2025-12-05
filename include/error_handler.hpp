#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <bits/stdc++.h>
using namespace std;

extern bool WRONG_FOUND_IN_LEXER;
extern bool WRONG_FOUND_IN_PARSER;

// 输出红色字符串
template <typename T>
string red(const T &input) {
    ostringstream oss;
    oss << "\033[31m" << input << "\033[0m";
    return oss.str();
}

// 输出绿色字符串
template <typename T>
string green(const T &input) {
    std::ostringstream oss;
    oss << "\033[32m" << input << "\033[0m";
    return oss.str();
}

// 输出红色错误信息。 A:词法错误 B:语法错误
void Err(char type, int line, string desc = "");

// 输出绿色正确提示信息
void Ok(string desc);

// Flex错误处理函数
void flex_error_handler(int token);

// Bison错误处理函数
void bison_error_handler(const char *msg = "");

#endif
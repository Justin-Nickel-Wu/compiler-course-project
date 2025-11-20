#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <bits/stdc++.h>
using namespace std;

struct Production {
    int lhs; // 产生式左部非终结符编号
    vector<int> rhs; // 产生式右部符号编号序列
    Production(): lhs(-1), rhs(vector<int>()) {}
};

#endif
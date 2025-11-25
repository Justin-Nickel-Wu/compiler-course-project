#ifndef LL1_H
#define LL1_H

#include <bits/stdc++.h>
using namespace std;

#include "Production.h"
#include "FirstFollowCalculator.h"

class LL1 {
private:
    Productions &prods;
    FirstFollowCalculator &ff;
    map<pair<int, int>, int> parse_table; // key: <非终结符, 终结符>, value: 产生式编号;
    
    void init();

public:
    LL1(Productions &productions, FirstFollowCalculator &ffcalc) : prods(productions), ff(ffcalc) {}
    bool bulid_parse_table();
    void output_parse_table(const string &filename);
};

#endif
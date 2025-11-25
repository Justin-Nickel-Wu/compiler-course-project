#ifndef FIRSTFOLLOWCALCULATOR_H
#define FIRSTFOLLOWCALCULATOR_H

#include <bits/stdc++.h>
using namespace std;

#include "Production.h"

class FirstFollowCalculator {
private:
    Productions &prods;
    vector<bool> __have_epsilon;
    vector<set<int>> First;
    vector<set<int>> Follow;
    
public:
    FirstFollowCalculator(Productions &productions) : prods(productions) {}
    void output_input_productions();
    void calculate_first();
    void calculate_follow();
    set<int>& get_First(int symbol_idx);
    set<int>& get_Follow(int symbol_idx);
    void output_first();
    void output_follow();
    bool have_epsilon(int idx);
};

#endif
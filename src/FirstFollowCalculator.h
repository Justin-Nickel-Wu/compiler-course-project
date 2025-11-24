#ifndef FIRSTFOLLOWCALCULATOR_H
#define FIRSTFOLLOWCALCULATOR_H

#include <bits/stdc++.h>
using namespace std;

#include "Production.h"

class FirstFollowCalculator {
private:
    Productions prods;
    vector<bool> have_epsilon;
    vector<set<int>> First;
    vector<set<int>> Follow;
    
public:
    void input(const string &filename){ prods.input(filename); }
    void output_input_productions();
    void calculate_first();
    void calculate_follow();
    void output_first();
    void output_follow();
};

#endif
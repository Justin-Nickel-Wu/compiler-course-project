#ifndef REMOVELEFTRECURSION_H
#define REMOVELEFTRECURSION_H

#include <bits/stdc++.h>
using namespace std;

#include "Production.h"

class RemoveLeftRecursion {
private:
    Productions input_prods;     // 存储所有产生式
    Productions new_prods; // 存储消除左递归后的产生式

public:
    void input(const string &filename){ input_prods.input(filename); }
    void eliminate_left_recursion();
    void output_input_productions();
    void output_processed_productions();
};

#endif
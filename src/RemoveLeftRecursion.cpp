#include "RemoveLeftRecursion.h"

void RemoveLeftRecursion::eliminate_left_recursion(){
/* 伪代码
    for i: 待替换产生式集合 // 所有左部相同的产生式
    begin
        que A, B
        A.push(i) 
        for j: 已处理产生式集合 // 所有左部相同的产生式
        begin
            while (A非空)
                if (A的队头需要替换)
                    使用j的产生式进行替换，结果入B
                    A.pop()
                else
                    直接将A的队头入B
                    A.pop()
            swap(A, B)
        end
        消除A中的左递归
        结果放入已处理产生式集合
    end
*/
    new_prods.init(input_prods);

    queue<Production> prods_queue;
    queue<Production> new_prods_queue;

    for (int i = 0; i < input_prods.size(); i++){ // 处理每一条产生式，消除左递归

        prods_queue.push(input_prods[i]);
        if (i + 1 < input_prods.size() && input_prods[i + 1].lhs == input_prods[i].lhs)
            continue; // 处理完同一非终结符的所有产生式后再进行下一步

        Production prod = input_prods[i];
        int j = 0;
        while (j < new_prods.size()){ // 执行替换
            if (new_prods[j].lhs == prod.lhs)
                break;

            int k = j;
            vector<Production> Aj_prods; // 把相同左部的产生式取出来
            while (k < new_prods.size() && new_prods[j].lhs == new_prods[k].lhs)
                Aj_prods.push_back(new_prods[k]), k++;
            j = k;

            // cout << "进行替换，使用非终结符: " << new_prods.get_token(Aj_prods[0].lhs) << endl;
            // for (auto Aj: Aj_prods)
            //     new_prods.output_production(Aj);

            while (!prods_queue.empty()){
                Production result(prod.lhs), front = prods_queue.front();
                prods_queue.pop();
                if (front.rhs[0] == Aj_prods[0].lhs){ // 需要替换
                    for (auto Aj: Aj_prods){
                        // debug
                        // cout << "替换产生式: ";
                        // new_prods.output_production(front);
                        // cout << "使用产生式: ";
                        // new_prods.output_production(Aj);

                        result.rhs.reserve(front.rhs.size() -1 + Aj.rhs.size());
                        if (Aj.rhs.size() != 1 || Aj.rhs[0] != input_prods.get_idx("ε")) // Aj产生式为ε, 则不添加Aj.rhs
                            result.rhs.insert(result.rhs.end(), Aj.rhs.begin(), Aj.rhs.end());
                        result.rhs.insert(result.rhs.end(), front.rhs.begin() + 1, front.rhs.end());
                        new_prods_queue.push(result);   
                        result.rhs.clear();
                    } 
                } else { // 不需要替换
                    result = front;
                    new_prods_queue.push(result);
                }   
            }
            swap(prods_queue, new_prods_queue);
        }


        bool have_left_recursion = false;
        vector<Production> temp;

        while (!prods_queue.empty()){
            temp.push_back(prods_queue.front());
            prods_queue.pop();
            if (temp.back().lhs == temp.back().rhs[0]) // 判断是否存在左递归
                have_left_recursion = true;
        }
        // debug temp内容
        // cout << "待处理产生式集合：" << endl;
        // for (auto p: temp)
        //     new_prods.output_production(p);
        // cout << endl;

        if (have_left_recursion){ // 如果存在左递归，进行消除
            string token = new_prods.get_token(prod.lhs) + "'";
            new_prods.new_token(token);
            new_prods.set_non_terminal(token);
            int new_lhs = new_prods.get_idx(token);
            //A  → A a | b | c =>
            //  A  → b A' | c A'
            //  A' → a A' | ε
            for (auto p: temp){
                // debug输出处理过程
                // output_production(p, "处理产生式：")
                // cout << "处理产生式: " << endl;
                // cout << p.lhs << ' ' << p.rhs.size() << endl;
                // cout << p.lhs << ' ' << p.rhs[0] << endl;
                if (p.lhs != p.rhs[0]){
                    Production new_prod(p.lhs, p.rhs.size() + 1);
                    new_prod.rhs.insert(new_prod.rhs.end(), p.rhs.begin(), p.rhs.end());
                    new_prod.rhs.push_back(new_lhs);
                    new_prods.push_back(new_prod);
                } else {
                    Production new_prod(new_lhs, p.rhs.size());
                    new_prod.rhs.insert(new_prod.rhs.end(), p.rhs.begin() + 1, p.rhs.end());
                    new_prod.rhs.push_back(new_lhs);
                    new_prods.push_back(new_prod);
                }
            }
            // 添加ε产生式
            Production eps_prod(new_lhs, 1);
            eps_prod.rhs.push_back(new_prods.get_idx("ε"));
            new_prods.push_back(eps_prod);

        } else { // 不存在左递归，直接加入new_productions
            for (auto p: temp){
                new_prods.push_back(p);
            }
        }
    }

    new_prods.sort();
}

void RemoveLeftRecursion::output_input_productions(){
    input_prods.output_productions("输入的产生式如下：");
}

void RemoveLeftRecursion::output_processed_productions(){
    new_prods.output_productions("消除左递归后的产生式如下：");
}

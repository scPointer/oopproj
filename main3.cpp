#include <iostream>
#include <sstream>
#include <cstdio>
#include "Node.h"
#include "build_tree.h"
#include "CG_debug.h"
#include "evaluation.h"
using namespace std;
int main()
{
    map<string, Node*> my_map;
    vector<double> set_answer_value;

    int m;
    cin >> m;
    getchar();
    for(int T = 1; T <= m; T++)
    {
        int n;
        cin >> n;
        stringstream vars, nodes, opts;
        nodes << "y =";
        for(int i = n; i >= 0; i--)
        {
            string ai;//x^i 的系数
            cin >> ai;
            vars << 'a' << i << " C " << ai << endl;
            if(i < n)
            {
                nodes << " +";
            }
            nodes << " a" << i;
            for(int j = 1; j <= i; j++)
            {
                nodes << " * x";
            }
        }
        string x;//x的初始值
        cin >> x;
        vars << 'x' << " V " << x << endl;
        nodes << endl << "new_x = x - y / ( GRAD y ) AT x" << endl;

        const int calc_times  = 5;
        for(int i = 1; i <= calc_times - 1; i++)
        {
            opts << "EVAL new_x" << endl;
            opts << "SETANSWER x " << 2 * i - 1 << endl;
        }
        opts << "EVAL new_x" << endl;

        string buffer;
        for(int i = 1; i <= n + 2; i++)
        {
            getline(vars, buffer);
            build_var(buffer, my_map);
        }
        for(int i = 1; i <= 2; i++)
        {
            getline(nodes, buffer);
            build_tree(buffer, my_map);
        }
        int counts = calc_times * 2 - 1;
        for(int i = 1; i <= counts; i++)
        {
            getline(opts, buffer);
            double answer;
            if(Compute(buffer, my_map, set_answer_value, answer))
            {
                printf("%.4lf%c", answer, " \n"[i == counts]);
            }
            set_answer_value.push_back(answer);
        }
        my_map.clear();
        set_answer_value.clear();
    }
}
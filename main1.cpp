#include <iostream>
#include <cstdio>
#include "Node.h"
#include "build_tree.h"
#include "CG_debug.h"
#include "evaluation.h"
using namespace std;
int main()
{
    int n, m, q;
    string buffer;
    map<string, Node*> my_map;
    vector<Node*> all_nodes;
    vector<double> set_answer_value;
    cin >> n;
    getchar();
    for(int i = 0; i < n; i++)
    {

        getline(cin, buffer);
        build_var(buffer, my_map, all_nodes);
    }
    cin >> m;
    getchar();
    for(int i = 0; i < m; i++)
    {
        getline(cin, buffer);
        build_tree(buffer, my_map, all_nodes);
    }
    cin >> q;
    getchar();
    for(int i = 0; i < q; i++)
    {
        getline(cin, buffer);
        double answer;
        if(Compute(buffer, my_map, set_answer_value, answer))
        {
            printf("%.4lf\n", answer);
        }
        set_answer_value.push_back(answer);
    }
    free_nodes(all_nodes);
}
#ifndef __COMPUTATIONALGRAPH_BUILD_TREE_H__
#define __COMPUTATIONALGRAPH_BUILD_TREE_H__

#include <string>
#include <map>
#include <vector>
#include "Node.h"
#include "evaluation.h"

void build_var(std::string s, std::map < std::string, Node* >& Var_map, std::vector<Node*>& all_nodes);
Node* create_calculator(std::string s, int& count_arg);
void build_tree(std::string s, std::map < std::string, Node* >& Var_map, std::vector<Node*>& all_nodes);
Node* connect(std::vector<std::string> s, std::map<std::string, Node*>& Var_map,
                int head, int tail, bool& is_legal, std::vector<Node*>& all_nodes);
void free_nodes(std::vector<Node*>& all_nodes);
#endif
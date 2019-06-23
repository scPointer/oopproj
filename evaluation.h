#ifndef __COMPUTATIONALGRAPH_EVALUATION_H__
#define __COMPUTATIONALGRAPH_EVALUATION_H__
#include <vector>
#include "Node.h"
#include <string>

bool string2double(double& v, std::string s);
bool Compute(std::string s, std::map<std::string, Node*>& Var_map,
             std::vector<double> setanswer, double& answer);

#endif
#ifndef __COMPUTATIONALGRAPH_EVALUATION_H__
#define __COMPUTATIONALGRAPH_EVALUATION_H__
#include <vector>
#include "Node.h"
#include <string>

double get_value ( Node* N ) ;
bool eval ( double v , Node* N ) ;

bool Compute ( std::string s , std::map < std::string , Node* > Var_map , std::vector < double > setanswer , double& answer );
double com ( Node* N , bool& is_legal ) ;

#endif
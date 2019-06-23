#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include <cstdio>
#include "evaluation.h"
#include "Node.h"
#include "CG_debug.h"

using namespace std;

//Node类

void Node::add_next(Node* N)
{
    Node::next.push_back(N);
}
void Node::del()
{
    int len = next.size();
    for(int i = 0; i < len; i ++)
    {
        delete next [ i ];
    }
    vector < Node* > ().swap(next);
}

void Node::init()
{
    for(auto& i : next)
    {
        i->init();
    }
}

void Placeholder::eval(double v)
{
    value = v;
    have_value = true;
}

void Placeholder::init()
{
    have_value = false;
    for(auto& i : next)
    {
        i->init();
    }
}

void Var::init()
{
    if(have_value == false && check_grad_empty())
        return;
    have_value = false;
    clear_grad();
    for(auto& i : next)
    {
        i->init();
    }
}

void ValueNode::print_var_name()
{
    cout << "PRINT operator: " <<  var_name << " = ";
}

std::string Var::get_name()
{
    return "Var";
}
std::string Placeholder::get_name()
{
    return "Placeholder";
}
std::string Variable::get_name()
{
    return "Variable";
}
std::string Constant::get_name()
{
    return "Constant";
}
std::string Unary_Operator::get_name()
{
    return "Unary_Operator";
}
std::string Binary_Operator::get_name()
{
    return "Binary_Operator";
}
std::string Ternary_Operator::get_name()
{
    return "Ternary_Operator";
}
std::string Grad_Operator::get_name()
{
    return "Grad_Operator";
}
std::string At_Operator::get_name()
{
    return "At_Operator";
}
//运算符类

double Unary_Operator::cal(double v, bool& is_legal)
{
    if(cal_name == "SIN")
    {
        return sin(v);
    }
    else if(cal_name == "EXP")
    {
        return exp(v);
    }
    else if(cal_name == "TANH")
    {
        return (exp(v) - exp(-v)) / (exp(v) + exp(-v));
    }
    else if(cal_name == "SIGMOID")
    {
        return 1.0 / (1.0 + exp(-v));
    }
    else if(cal_name == "LOG")
    {
        if(v > 0)
        {
            return log(v);
        }
        else
        {
            is_legal = false;
            throw_error(NON_POSITIVE_VALUE_IN_LOG);
            return 0.0;
        }
    }
    else if(cal_name == "PRINT")
    {
        ValueNode* valn = dynamic_cast < ValueNode* > (next[0]);
        if(!valn)
        {
            is_legal = false;
            throw_error(UNKNOWN_NODE_TYPE, next[0]->get_name() + "\0");
            return 0.0;
        }
        valn->print_var_name();
        printf("%.4lf\n", v);
        return v;
    }
    else if(cal_name == "ASSERT")
    {
        if(v < eps)
        {
            is_legal = false;
            throw_error(ASSERTION_FAILED);
        }
        return 0.0;
    }
    else
    {
        is_legal = false;
        throw_error(NO_MATCH_OPERATOR_FOR, cal_name);
        return 0.0;
    }

    return 0.0;
}

double Binary_Operator::cal(double v1, double v2, bool& is_legal)
{
    if(cal_name == "+")
    {
        return v1 + v2;
    }
    else if(cal_name == "-")
    {
        return v1 - v2;
    }
    else if(cal_name == "*")
    {
        return v1 * v2;
    }
    else if(cal_name == "/")
    {
        if(v2 == 0)
        {
            is_legal = false;
            throw_error(DEVISION_BY_ZERO);
            return 0.0;
        }
        else
        {
            return v1 / v2;
        }
    }
    else if(cal_name == "<")
    {
        if(v1 < v2)
        {
            return 1.0;
        }
        else
        {
            return 0.0;
        }
    }
    else if(cal_name == ">")
    {
        if(v1 > v2)
        {
            return 1.0;
        }
        else
        {
            return 0.0;
        }
    }
    else if(cal_name == "<=")
    {
        if(v1 <= v2)
        {
            return 1.0;
        }
        else
        {
            return 0.0;
        }
    }
    else if(cal_name == ">=")
    {
        if(v1 >= v2)
        {
            return 1.0;
        }
        else
        {
            return 0.0;
        }
    }
    else if(cal_name == "==")
    {
        if(v1 == v2)
        {
            return 1.0;
        }
        else
        {
            return 0.0;
        }
    }
    else if(cal_name == "BIND")
    {
        return v1;
    }
    else if(cal_name == "=")
    {
        is_legal = false;
        throw_error(UNNECESSARY_ASSIGNMENT);
        return 0.0;
    }
    is_legal = false;
    throw_error(NO_MATCH_OPERATOR_FOR, cal_name);
    return 0.0;
}

double Ternary_Operator::cal(double v1, double v2, double v3, bool& is_legal)
{
    if(cal_name == "COND")
    {
        return v1 > 0 ? v2 : v3;
    }
    else
    {
        is_legal = false;
        throw_error(NO_MATCH_OPERATOR_FOR, cal_name);
        return 0.0;
    }
    return 0.0;
}

double At_Operator::cal(bool& is_legal)
{
    Grad_Operator* grad_node = dynamic_cast<Grad_Operator*>(next[0]);
    if(grad_node)
    {
        return grad_node->grad_at(next[1], is_legal);
    }
    else
    {
        Var* var_node = dynamic_cast<Var*>(next[0]);
        if(var_node)
        {
            return var_node->compute_at(next[1], is_legal);
        }
        else
        {
            is_legal = false;
            throw_error(GRAD_OPERATOR_NOT_FOUND);
            return 0.0;
        }
    }
}

double Placeholder::grad(Node* target, bool& is_legal)
{
    if(target == this)
    {
        return 1.0;
    }
    else
    {
        return 0.0;
    }
}

double Constant::grad(Node* target, bool& is_legal)
{
    if(target == this)
    {
        return 1.0;
    }
    else
    {
        return 0.0;
    }
}

double Var::grad(Node* target, bool& is_legal)
{
    if(target == this)
    {
        return 1.0;
    }

    std::unordered_map<Node*, double>::iterator iter = grad_of.find(target);
    if(iter != grad_of.end())
    {
        return iter -> second;
    }
    else
    {
        double ret = next[0]->grad(target, is_legal);
        grad_of[target] = ret;
        return ret;
    }
}

double Variable::grad(Node* target, bool& is_legal)
{
    if(target == this)
    {
        return 1.0;
    }
    else
    {
        return 0.0;
    }
}

double Unary_Operator::grad(Node* target, bool& is_legal)
{
    double v = next[0]->calculate(is_legal);
    if(cal_name == "SIN")
    {
        return cos(v) * next[0]->grad(target, is_legal);
    }
    else if(cal_name == "EXP")
    {
        return exp(v) * next[0]->grad(target, is_legal);
    }
    else if(cal_name == "TANH")
    {
        return 4.0 / (exp(2 * v) + exp(-2 * v) + 2) * next[0]->grad(target,
                is_legal);
    }
    else if(cal_name == "SIGMOID")
    {
        return 1.0 / (exp(v) + exp(v) + 2) * next[0]->grad(target, is_legal);
    }
    else if(cal_name == "LOG")
    {
        if(v > 0)
        {
            return (1.0 / v) * next[0]->grad(target, is_legal);
        }
        else
        {
            is_legal = false;
            throw_error(DEVISION_BY_ZERO);
            return 0.0;
        }
    }
    else if(cal_name == "PRINT")
    {
        ValueNode* valn = dynamic_cast < ValueNode* > (next[0]);
        if(!valn)
        {
            is_legal = false;
            throw_error(UNKNOWN_NODE_TYPE, next[0]->get_name() + "\0");
            return 0.0;
        }
        valn->print_var_name();
        return next[0]->grad(target, is_legal);
    }
    else if(cal_name == "ASSERT")
    {
        if(v < eps)
        {
            is_legal = false;
            throw_error(ASSERTION_FAILED);
        }
        return 0.0;
    }
    else
    {
        is_legal = false;
        throw_error(NO_MATCH_OPERATOR_FOR, cal_name);
        return 0.0;
    }
}

double Binary_Operator::grad(Node* target, bool& is_legal)
{
    if(cal_name == "+")
    {
        return next[0]->grad(target, is_legal) + next[1]->grad(target, is_legal);
    }
    else if(cal_name == "-")
    {
        return next[0]->grad(target, is_legal) - next[1]->grad(target, is_legal);
    }
    else if(cal_name == "*")
    {
        double v_left = next[0]->calculate(is_legal);
        double v_right = next[1]->calculate(is_legal);
        return v_left * next[1]->grad(target,
                                      is_legal) + v_right * next[0]->grad(target, is_legal);
    }
    else if(cal_name == "/")
    {
        double v_left = next[0]->calculate(is_legal);
        double v_right = next[1]->calculate(is_legal);
        if(v_right == 0)
        {
            is_legal = false;
            throw_error(DEVISION_BY_ZERO);
            return 0.0;
        }
        double numerator = v_right * next[0]->grad(target,
                           is_legal) - v_left * next[1]->grad(target, is_legal);
        return numerator / (v_right * v_right);
    }
    else if(cal_name == "<" || cal_name == ">" || cal_name == "<=" ||
            cal_name == ">=" || cal_name == "==")
    {
        return cal(next[0]->calculate(is_legal), next[1]->calculate(is_legal), is_legal);
    }
    else if(cal_name == "BIND")
    {
        return next[0]->grad(target, is_legal);
    }
    else if(cal_name == "=")
    {
        is_legal = false;
        throw_error(UNNECESSARY_ASSIGNMENT);
        return 0.0;
    }
    is_legal = false;
    throw_error(NO_MATCH_OPERATOR_FOR, cal_name);
    return 0.0;
}

double Ternary_Operator::grad(Node* target, bool& is_legal)
{
    if(cal_name == "COND")
    {
        double v1 = next[0]->calculate(is_legal);
        return v1 > 0 ? next[1]->grad(target, is_legal) : next[2]->grad(target, is_legal);
    }
    else
    {
        is_legal = false;
        throw_error(NO_MATCH_OPERATOR_FOR, cal_name);
        return 0.0;
    }
}

double Grad_Operator::grad(Node* target, bool& is_legal)
{
    is_legal = false;
    throw_error(TRY_HIGHER_DERIVATIVE);
    return 0.0;
}

double At_Operator::grad(Node* target, bool& is_legal)
{
    is_legal = false;
    throw_error(TRY_HIGHER_DERIVATIVE);
    return 0.0;
}

double Var::compute_at(Node* target, bool& is_legal)
{
    have_value = true;
    Grad_Operator* grad_node = dynamic_cast<Grad_Operator*>(next[0]);
    if(grad_node)
    {
        return grad_node->grad_at(target, is_legal);
    }
    else
    {
        Var* var_node = dynamic_cast<Var*>(next[0]);
        if(var_node)
        {
            return var_node->compute_at(target, is_legal);
        }
        else
        {
            is_legal = false;
            throw_error(GRAD_OPERATOR_NOT_FOUND);
            return 0.0;
        }
    }

}

bool Var::check_grad_empty()
{
    return grad_of.empty();
}

void Var::clear_grad()
{
    grad_of.clear();
}

double Placeholder::calculate(bool& is_legal)
{
    if(have_value)
    {
        return value;
    }
    else
    {
        is_legal = false;
        throw_error(PLACEHOLDER_MISSING);
        return 0.0;
    }
}

double Constant::calculate(bool& is_legal)
{
    return value;
}

double Variable::calculate(bool& is_legal)
{
    return value;
}

double Var::calculate(bool& is_legal)
{
    if(have_grad_node)   // 用户直接对 GRAD 求值是非法的。
    {
        is_legal = false;
        throw_error(AT_OPERATOR_NOT_FOUND);
        return 0.0;
    }
    else if(have_value)    //已被计算过
    {
        return value;
    }
    else //未被计算过
    {
        double v = next[0]->calculate(is_legal);     //计算
        if(!is_legal)    //无法计算
        {
            have_value = true;
            return 0.0;
        }
        else  //尝试赋值
        {
            value = v; 
            have_value = true;
            return v;
        }
    }
}

double Grad_Operator::calculate(bool& is_legal)
{
    is_legal = false;
    throw_error(AT_OPERATOR_NOT_FOUND);
    return 0.0;
}

double Unary_Operator::calculate(bool& is_legal)
{
    double v0 = next[0]->calculate(is_legal);     //递归计算
    if(!is_legal)
    {
        return 0.0;
    }
    double v;
    v = cal(v0, is_legal);
    if(!is_legal)
    {
        return 0.0;    //无法计算
    }
    return v;
}

double At_Operator::calculate(bool& is_legal)
{
    return cal(is_legal);
}

double Binary_Operator::calculate(bool& is_legal)
{
    double v1 = next[0]->calculate(is_legal);  //计算第一个值
    if(!is_legal)
    {
        return 0.0;
    }
    double v2 = next[1]->calculate(is_legal);  //计算第二个值
    if(!is_legal)
    {
        return 0.0;
    }
    double v = cal(v1, v2, is_legal);  //计算当前运算符
    if(!is_legal)
    {
        return 0.0;
    }
    return v;
}

double Ternary_Operator::calculate(bool& is_legal)
{
    double v1 = next[0]->calculate(is_legal);     //计算第一个值
    if(!is_legal)
    {
        return 0.0;
    }
    double v2 = next[1]->calculate(is_legal);     //计算第二个值
    if(!is_legal)
    {
        return 0.0;
    }
    double v3 = next[2]->calculate(is_legal);     //计算第三个值
    if(!is_legal)
    {
        return 0.0;
    }
    double v = cal(v1, v2, v3, is_legal);        //计算
    return v;
}

double Grad_Operator::grad_at(Node* target, bool& is_legal)
{
    return next[0]->grad(target, is_legal);
}

int OperatorNode::get_priority(std::string c)
{
    if(c == "<")
    {
        return 1;
    }
    if(c == ">")
    {
        return 1;
    }
    if(c == "<=")
    {
        return 1;
    }
    if(c == ">=")
    {
        return 1;
    }
    if(c == "==")
    {
        return 1;
    }
    if(c == "+")
    {
        return 2;
    }
    if(c == "-")
    {
        return 2;
    }
    if(c == "*")
    {
        return 3;
    }
    if(c == "/")
    {
        return 3;
    }
    if(c == "AT")
    {
        return 4;
    }
    if(c == "SIN")
    {
        return 5;
    }
    if(c == "LOG")
    {
        return 5;
    }
    if(c == "EXP")
    {
        return 5;
    }
    if(c == "SIGMOID")
    {
        return 5;
    }
    if(c == "TANH")
    {
        return 5;
    }
    if(c == "PRINT")
    {
        return 6;
    }
    if(c == "ASSERT")
    {
        return 6;
    }
    if(c == "COND")
    {
        return 6;
    }
    if(c == "BIND")
    {
        return 6;
    }
    if(c == "GRAD")
    {
        return 6;
    }
    return 0;
}
//#end#

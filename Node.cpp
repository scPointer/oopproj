#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include <cstdio>
#include "evaluation.h"
#include "Node.h"
#include "CG_debug.h"

using namespace std ;

map < string , Node* > Var_map ;

//Node类

void Node::add_next ( Node* N ) { Node::next.push_back ( N ) ; }
void Node::del ()
{
    int len = next.size () ;
    for ( int i = 0 ; i < len ; i ++ ) delete next [ i ] ;
    vector < Node* > ().swap ( next ) ;
}

std::string Var::get_name () { return "Var" ; }
std::string Placeholder::get_name () { return "Placeholder" ; }
std::string Variable::get_name () { return "Variable" ; }
std::string Constant::get_name () { return "Constant" ; }
std::string Unary_Operator::get_name () { return "Unary_Operator" ; }
std::string Binary_Operator::get_name () { return "Binary_Operator" ; }
std::string Ternary_Operator::get_name () { return "Ternary_Operator" ; }
std::string Grad_Operator::get_name () { return "Grad_Operator" ; }
std::string At_Operator::get_name () { return "At_Operator" ; }
//运算符类

double Unary_Operator::cal ( string s , double v , bool& is_legal , string print_out )
{
    if ( s == "SIN" ) return sin ( v ) ;
    else if ( s == "EXP" ) return exp ( v ) ;
    else if ( s == "TANH" ) return ( exp ( v ) - exp ( -v ) ) / ( exp ( v ) + exp ( -v ) ) ;
    else if ( s == "SIGMOID" ) return 1.0 / ( 1.0 + exp ( -v ) ) ;
    else if ( s == "LOG" )
    {
        if ( v > 0 ) return log ( v ) ;
        else 
        {
            is_legal = false ;
            throw_error ( NON_POSITIVE_VALUE_IN_LOG ) ;
            return 0.0 ;
        }
    }
    else if ( s == "PRINT" )
    {
        cout << "PRINT operator: " << print_out << " = " ;
        printf ( "%.4lf\n" , v ) ;
        return v ;
    }
    else if( s == "ASSERT" )
    {
        if( v < eps )
        {
            is_legal = false;
            throw_error( ASSERTION_FAILED ) ;
        }
        return 0.0 ;
    }
    else
    {
        is_legal = false ;
        throw_error ( NO_MATCH_OPERATOR_FOR , s ) ;
        return 0.0 ;
    }
    
    return 0.0 ;
}

double Binary_Operator::cal ( string s , double v1 , double v2 , bool& is_legal )
{
    if ( s == "+" ) return v1 + v2 ;
    else if ( s == "-" ) return v1 - v2 ;
    else if ( s == "*" ) return v1 * v2 ;
    else if ( s == "/" )
    {
        if ( v2 == 0 )
        {
            is_legal = false ;
            throw_error ( DEVISION_BY_ZERO ) ;
            return 0.0 ;
        }
        else return v1 / v2 ;
    }
    else if ( s == "<" )
    {
        if ( v1 < v2 ) return 1.0 ;
        else return 0.0 ;
    }
    else if ( s == ">" )
    {
        if ( v1 > v2 ) return 1.0 ;
        else return 0.0 ;
    }
    else if ( s == "<=" )
    {
        if ( v1 <= v2 ) return 1.0 ;
        else return 0.0 ;
    }
    else if ( s == ">=" )
    {
        if ( v1 >= v2 ) return 1.0 ;
        else return 0.0 ;
    }
    else if ( s == "==" )
    {
        if ( v1 == v2 ) return 1.0 ;
        else return 0.0 ;
    }
    else if( s == "BIND" )
    {
        return v1 ;
    }
    else if ( s == "=" )
    {
        is_legal = false ;
        throw_error ( UNNECESSARY_ASSIGNMENT ) ;
        return 0.0 ;
    }
    is_legal = false ;
    throw_error ( NO_MATCH_OPERATOR_FOR , s ) ;
    return 0.0 ;
}

double Ternary_Operator::cal ( string s , double v1 , double v2 , double v3 , bool& is_legal )
{
    if ( s == "COND" )
    {
        return v1 > 0 ? v2 : v3 ;
    }
    else
    {
        is_legal = false ;
        throw_error ( NO_MATCH_OPERATOR_FOR , s ) ;
        return 0.0 ;
    }
    return 0.0 ;
}

double Grad_Operator::cal(Node* target, bool& is_legal)
{
    return next[0]->grad(target, is_legal);
}

double At_Operator::cal(bool& is_legal)
{
    Grad_Operator* grad_node = dynamic_cast<Grad_Operator*> (next[0]);
    if(grad_node)
        return grad_node->cal(next[1], is_legal);
    else
    {
        Var* var_node = dynamic_cast<Var*> (next[0]);
        if(var_node)
            return var_node->compute_at(next[1], is_legal);
        else
        {
            is_legal = false;
            throw_error( GRAD_OPERATOR_NOT_FOUND );
            return 0.0;
        }
    }
}

double Placeholder::grad(Node* target, bool& is_legal)
{
    if(target == this)
        return 1.0;
    else
        return 0.0;
}

double Constant::grad(Node* target, bool& is_legal)
{
    if(target == this)
        return 1.0;
    else
        return 0.0;
}

double Var::grad(Node* target, bool& is_legal)
{
    if(target == this)
        return 1.0;
    
    std::unordered_map<Node*, double>::iterator iter = grad_of.find(target);
    if(iter != grad_of.end())
        return iter -> second;
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
        return 1.0;
    else
        return 0.0;
}

double Unary_Operator::grad(Node* target, bool& is_legal)
{
    double v = com(next[0], is_legal);
    if(cal_name == "SIN")
        return cos( v ) * next[0]->grad(target, is_legal);
    else if(cal_name == "EXP")
        return exp( v ) * next[0]->grad(target, is_legal);
    else if(cal_name == "TANH")
        return 4.0 / (exp(2 * v) + exp(-2 * v) + 2) * next[0]->grad(target, is_legal);
    else if(cal_name == "SIGMOID")
        return 1.0 / (exp(v) + exp(v) + 2) * next[0]->grad(target, is_legal);
    else if(cal_name == "LOG")
    {
        if( v > 0 )
            return (1.0 / v) * next[0]->grad(target, is_legal);
        else
        {
            is_legal = false;
            throw_error( DEVISION_BY_ZERO ) ;
            return 0.0;
        }
    }
    else if(cal_name == "PRINT")
    {
        cout << "PRINT operator: " << get_var_name(next[0]) << " = " ;
        printf ( "%.4lf\n" , v ) ;
        return next[0]->grad(target, is_legal) ;
    }
    else if(cal_name == "ASSERT" )
    {
        if( v < eps )
        {
            is_legal = false;
            throw_error( ASSERTION_FAILED ) ;
        }
        return 0.0 ;
    }
    else
    {
        is_legal = false ;
        throw_error ( NO_MATCH_OPERATOR_FOR , cal_name ) ;
        return 0.0 ;
    }
}

double Binary_Operator::grad(Node* target, bool& is_legal)
{
    if(cal_name == "+")
        return next[0]->grad(target, is_legal) + next[1]->grad(target, is_legal);
    else if(cal_name == "-")
        return next[0]->grad(target, is_legal) - next[1]->grad(target, is_legal);
    else if(cal_name == "*")
    {
        double v_left = com(next[0], is_legal);
        double v_right = com(next[1], is_legal);
        return v_left * next[1]->grad(target, is_legal) + v_right * next[0]->grad(target, is_legal);
    }
    else if(cal_name == "/")
    {
        double v_left = com(next[0], is_legal);
        double v_right = com(next[1], is_legal);
        if ( v_right == 0 )
        {
            is_legal = false ;
            throw_error ( DEVISION_BY_ZERO ) ;
            return 0.0 ;
        }
        double numerator = v_right * next[0]->grad(target, is_legal) - v_left * next[1]->grad(target, is_legal);
        return numerator / (v_right * v_right);
    }
    else if(cal_name == "<" || cal_name == ">" || cal_name == "<=" || cal_name == ">=" || cal_name == "==")
    {
        return cal(cal_name, com(next[0], is_legal), com(next[1], is_legal), is_legal);
    }
    else if(cal_name == "BIND")
    {
        return next[0]->grad(target, is_legal);
    }
    else if (cal_name == "=")
    {
        is_legal = false ;
        throw_error ( UNNECESSARY_ASSIGNMENT ) ;
        return 0.0 ;
    }
    is_legal = false ;
    throw_error ( NO_MATCH_OPERATOR_FOR , cal_name ) ;
    return 0.0 ;
}

double Ternary_Operator::grad(Node* target, bool& is_legal)
{
    if(cal_name == "COND")
    {
        double v1 = com(next[0], is_legal);
        return v1 > 0? next[1]->grad(target, is_legal): next[2]->grad(target, is_legal);
    }
    else
    {
        is_legal = false ;
        throw_error ( NO_MATCH_OPERATOR_FOR , cal_name ) ;
        return 0.0 ;
    }
}

double Grad_Operator::grad(Node* target, bool& is_legal)
{
    is_legal = false;
    throw_error( TRY_HIGHER_DERIVATIVE );
    return 0.0;
}

double At_Operator::grad(Node* target, bool& is_legal)
{
    is_legal = false;
    throw_error( TRY_HIGHER_DERIVATIVE );
    return 0.0;
}

double Var::compute_at(Node* target, bool& is_legal)
{
    have_value = true;
    Grad_Operator* grad_node = dynamic_cast<Grad_Operator*> (next[0]);
    if(grad_node)
        return grad_node->cal(target, is_legal);
    else
    {
        Var* var_node = dynamic_cast<Var*> (next[0]);
        if(var_node)
            return var_node->compute_at(target, is_legal);
        else
        {
            is_legal = false;
            throw_error( GRAD_OPERATOR_NOT_FOUND );
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

//#end#
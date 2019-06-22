#include "Node.h"
#include "CG_debug.h"
#include "build_tree.h"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <iostream>

double get_value ( Node* N ) //必须确保N指向的是Var类或Placeholder类或Constant类或Variable类
{
    std::string s = N -> get_name () ;
    if ( s == "Var" )
    {
        Var* v = dynamic_cast < Var* > ( N ) ;
        return v -> value ;
    }
    else if ( s == "Placeholder" )
    {
        Placeholder* p = dynamic_cast < Placeholder* > ( N ) ;
        return p -> value ;
    }
    else if ( s == "Constant" )
    {
        Constant* c = dynamic_cast < Constant* > ( N ) ;
        return c -> value ;
    }
    else if ( s == "Variable" )
    {
        Variable* vc = dynamic_cast < Variable* > ( N ) ;
        return vc -> value ;
    }
    throw_error ( 12 , s ) ;
    return 0.0 ;
}

bool eval ( double v , Node* N ) //必须确保N指向的是Var类或Placeholder类
{
    std::string s = N -> get_name () ;
    if ( s == "Var" )
    {
        Var* var = dynamic_cast < Var* > ( N ) ;
        var -> value = v ;
        return true ;
    }
    if ( s == "Placeholder" )
    {
        Placeholder* p = dynamic_cast < Placeholder* > ( N ) ;
        p -> value = v ;
        return true ;
    }
    throw_error ( 6 ) ;
    return false ;
}

std::string get_var_name ( Node* N )
{
    std::string name = N -> get_name () ;
    if ( name == "Placeholder" )
    {
        Placeholder* p = dynamic_cast < Placeholder* > ( N ) ;
        return p -> var_name ;
    }
    if ( name == "Var" )
    {
        Var* var = dynamic_cast < Var* > ( N ) ;
        return var -> var_name ;
    }
    if ( name == "Constant" )
    {
        Constant* c = dynamic_cast < Constant* > ( N ) ;
        return c -> var_name ;
    }
    if ( name == "Variable" )
    {
        Variable* vc = dynamic_cast < Variable* > ( N ) ;
        return vc -> var_name ;
    }
    throw_error ( 10 ) ;
    return "" ;
}

/**
 * Function Compute
 * 作用：输入一行待计算表达式，并将其计算结果返回
 * 传参： s：待处理字符串
 *       Var_map：结点映射，预处理输入字符串时需用其为结点赋值、清楚赋值
 *       setanswer：用于处理SETANSWER关键字，读取第i个结点第输出
 *       answer：将计算所得结果传出
 * 返回值：一个bool类型变量，表示该行表达式是否有输出。返回true表示表达式有输出，返回false表示表达式无输出
**/
bool Compute ( std::string s , std::map < std::string , Node* > Var_map , std::vector < double > setanswer , double& answer ) //输入一个字符串s,将其计算出来
{
    std::stringstream in ( s ) ; //将待处理字符串转换为字符串流，读取其中各个字符串
    std::string buf ; //从in流中读取字符串第临时变量
    std::vector < std::string > vec ; //储存表达式中字符串
    while ( in >> buf ) vec.push_back ( buf ) ; //读取字符串
    if ( vec.size() == 0 ) //空串，表达式不合法，抛出错误
    {
        throw_error ( 10 ) ;
        return false ;
    }
    if ( vec [ 0 ] == "SETCONSTANT" ) //判断表达式类型
    {
        if ( vec.size () < 3 ) //语法检查：防止不正规表达式
        {
            throw_error ( 10 ) ;
            return false ;
        }
        if ( Var_map.find ( vec [ 1 ] ) == Var_map.end() ) //语法检查：防止计算未出现过的结点
        {
            throw_error ( 11 , vec [ 1 ] ) ;
            return false ;
        }

        /**********语法检查**********/
        
        double v ;
        try
        {
            v = stod ( vec [ 2 ] ) ;
        }
        catch ( std::invalid_argument& )
        {
            throw_error ( 10 ) ;
            return false ;
        }
        catch ( std::out_of_range& )
        {
            throw_error ( 17 ) ;
            return false ;
        }
        
        /**********end**********/

        if ( Var_map [ vec [ 1 ] ] -> get_name () != "Variable" ) //语法检查：防止SET一个不正确的类型
        {
            throw_error ( 13 ) ;
            return false ;
        }
        //set操作
        Variable* vc = dynamic_cast < Variable* > ( Var_map [ vec [ 1 ] ] ) ;
        vc -> set ( v ) ;
        return false ;
    }
    else if ( vec [ 0 ] == "SETANSWER" ) //判断表达式类型
    {
        if ( vec.size() < 3 ) //语法检查：防止不正规表达式
        {
            throw_error ( 10 ) ;
            return false ;
        }
        if ( Var_map.find ( vec [ 1 ] ) == Var_map.end() ) //语法检查：防止计算未出现过的结点
        {
            throw_error ( 11 , vec [ 1 ] ) ;
            return false ;
        }
        

        /**********语法检查**********/
        
        double v ;
        try
        {
            v = stod ( vec [ 2 ] ) ;
        }
        catch ( std::invalid_argument& )
        {
            throw_error ( 10 ) ;
            return false ;
        }
        catch ( std::out_of_range& )
        {
            throw_error ( 17 ) ;
            return false ;
        }
        
        /**********end**********/

        if ( Var_map [ vec [ 1 ] ] -> get_name () != "Variable" ) //语法检查：防止SET一个不正确的类型
        {
            throw_error ( 13 ) ;
            return false ;
        }
        //set操作
        Variable* vc = dynamic_cast < Variable* > ( Var_map [ vec [ 1 ] ] ) ;
        vc -> set ( setanswer [ v - 1 ] ) ;
        return false ;
    }
    else if ( vec [ 0 ] == "EVAL" )
    {
        if ( Var_map.find(vec [ 1 ] ) == Var_map.end() ) //语法检查：防止计算一个未出现过的结点
        {
            throw_error ( 11 , vec [ 1 ] ) ;
            return false ;
        }
        if ( vec.size () >= 3 ) //确保前三个变量存在
        {

            /**********语法检查**********/

            int Placeholder_number ; //要赋值运算符个数
            try
            {
                Placeholder_number = stoi ( vec [ 2 ] ) ;
            }
            catch ( std::invalid_argument& )
            {
                throw_error ( 16 ) ;
                return false ;
            }
            catch ( std::out_of_range& )
            {
                throw_error ( 17 ) ;
                return false ;
            }

            /**********end**********/

            if ( Placeholder_number * 2 + 3 > vec.size() ) //语法检查：防止要赋值运算符个数与实际输入个数不同
            {
                throw_error ( 10 ) ;
                return false ;
            }
            for ( int i = 1 ; i <= Placeholder_number ; i ++ ) //给Placeholder结点赋值
            {
                std::string name = vec [ 2 + ( 2 * i ) - 1 ] ;
                if ( Var_map.find ( name ) == Var_map.end() ) //防止赋值未出现过的结点
                {
                    throw_error ( 11 , vec [ 2 + ( 2 * i ) - 1 ] ) ;
                    return false ;
                }
                Node* &N = Var_map [ name ] ;
                std::string type_name = N -> get_name () ;
                if ( type_name != "Placeholder" ) //防止赋值非Placeholder结点
                {
                    throw_error ( 14 ) ;
                    return false ;
                }
                double v ;

                /**********语法检查**********/

                try
                {
                    v = stod ( vec [ 2 + ( 2 * i ) ] ) ;
                }
                catch ( std::invalid_argument& )
                {
                    throw_error ( 10 ) ;
                    return false ;
                }
                catch ( std::out_of_range& )
                {
                    throw_error ( 17 ) ;
                    return false ;
                }

                /**********end**********/

                //赋值操作
                eval ( v , N ) ;
                Placeholder* p = dynamic_cast < Placeholder* > ( N ) ;
                p -> have_value = true ;
            }
        }
        bool is_legal = true ; //判定该表达式是否能合法计算
        answer = com ( Var_map [ vec [ 1 ] ] , is_legal ) ; //计算

        /**********清除已赋的值**********/

        init ( Var_map [ vec [ 1 ] ] ) ; //递归清除结点

        //此处再执行一次根据输入清除Placeholder结点目的是为了防止表达式给一些该运算不依赖的Placeholder赋值，从而导致Placeholder_Missing
        //错误不一定触发。
        if ( vec.size() >= 3 )
        {
            int Placeholder_number = stoi ( vec [ 2 ] ) ; //若vec[2]不是能转换成数字的字符串，在赋值时已经跳出函数，不会执行到此处
            //清除赋值
            for ( int i = 1 ; i <= Placeholder_number ; i ++ )
            {
                std::string name = vec [ 2 + ( 2 * i ) - 1 ] ;
                Placeholder* p = dynamic_cast < Placeholder* > ( Var_map [ name ] ) ;
                p -> have_value = false ;
            }
        }

        /**********结束清楚**********/

        return is_legal ; //若该表达式不能合法计算，则在函数内部抛出错误，不需要也无法输出
    }
    throw_error ( 10 ) ; //不合法表达式
    return false ;
}

/**
 * Function com
 * 坦白来说，我真的不知道该怎么命名这个函数了。就这样吧
 * 功能：通过已建好的表达式树进行计算
 * 传参：N：需要计算的结点值（不能为运算符结点）
 *      is_legal：表达式能否被合法地计算。若非法计算则为false
 * 返回值：计算所得结果
**/
double com ( Node* N , bool& is_legal )
{
    //递归计算
    int len=N->next.size(); //读取后继结点个数
    if(len==0) //叶子节点，只有三种类型
    {
        std::string name = N -> get_name() ;
        if ( name == "Placeholder" ) //Placeholder类
        {
            Placeholder* p = dynamic_cast < Placeholder* > ( N ) ;
            if ( p -> have_value ) return get_value ( N ) ; //Placeholder_Missing错误
            else
            {
                is_legal = false ;
                throw_error ( PLACEHOLDER_MISSING ) ;
                return 0.0 ;
            }
        }
        else //常数类
        {
            return get_value ( N ) ;
        }
        //由于建树时确保了无后继结点的类仅有这三种类，故不需考虑其他情况
    }
    else if(len==1) //一元运算符
    {
        std::string name = N -> get_name() ;
        if ( name == "Var" ) //Var结点特殊处理，判定是否已经被计算过
        {
            Var* var = dynamic_cast < Var* > ( N ) ;
            if ( var -> have_value ) return get_value ( N ) ; //已被计算过
            else //未被计算过
            {
                double v = com ( N -> next[0] , is_legal ) ; //计算
                if ( !is_legal ) return 0.0 ; //无法计算
                if ( eval ( v , N ) ) //尝试赋值
                {
                    var -> have_value = true ;
                    return v ;
                }
                else //此处其实已经保证了eval的是var结点，不会进入此else分支。但我就是想写这个else。。。
                {
                    is_legal = false ;
                    throw_error ( UNKNOWN_TYPE_ERROR ) ;
                    return 0.0 ;
                }
            }
        }
        else
        {
            Unary_Operator* una = dynamic_cast < Unary_Operator* > ( N ) ; //一元运算符
            double v0 = com ( N -> next[0] , is_legal ) ; //递归计算
            if ( !is_legal ) return 0.0 ;
            double v ;
            if ( una -> cal_name == "PRINT" ) //单独处理PRINT操作（比较特殊）
            {
                Node* n = una -> next [ 0 ] ;
                std::string next_name = n -> get_name () ;
                if ( next_name == "Placeholder" )
                {
                    Placeholder* p = dynamic_cast < Placeholder* > ( n ) ;
                    v = una -> cal ( una -> cal_name , v0 , is_legal , get_var_name ( N -> next [ 0 ] ) ) ;
                }
                else if ( next_name == "Var" )
                {
                    Var* var = dynamic_cast < Var* > ( n ) ;
                    v = una -> cal ( una -> cal_name , v0 , is_legal , get_var_name ( N -> next [ 0 ] ) ) ;
                }
                else if ( next_name == "Constant" )
                {
                    Constant* c = dynamic_cast < Constant* > ( n ) ;
                    v = una -> cal ( una -> cal_name , v0 , is_legal , get_var_name ( N -> next [ 0 ] ) ) ;
                }
                else if ( next_name == "Variable" )
                {
                    Variable* vc = dynamic_cast < Variable* > ( n ) ;
                    v = una -> cal ( una -> cal_name , v0 , is_legal , get_var_name ( N -> next [ 0 ] ) ) ;
                }
            }
            else v = una -> cal ( una -> cal_name , v0 , is_legal ) ; //调用cal函数运算
            if ( !is_legal ) return 0.0 ; //无法计算
            return v ;
        }
    }
    else if(len==2) //二元运算符
    {
        //建树保证两个后继结点的Node仅有Binary_Operator类型结点
        Binary_Operator* bin = dynamic_cast < Binary_Operator* > ( N ) ;
        if(bin -> cal_name == "AT")//求导运算特殊处理
        {

        }
        double v1 = com ( N -> next[0] , is_legal ) ; //计算第一个值
        if ( !is_legal ) return 0.0 ;
        double v2 = com ( N -> next[1] , is_legal ) ; //计算第二个值
        if ( !is_legal ) return 0.0 ;
        double v = bin -> cal ( bin->cal_name , v1 , v2 , is_legal ); //计算
        if ( !is_legal ) return 0.0 ;
        return v ;
    }
    else if(len==3) //三元运算符
    {
        Ternary_Operator* ter = dynamic_cast < Ternary_Operator* > ( N ) ;
        double v1 = com ( N -> next[0] , is_legal ) ; //计算第一个值
        if ( !is_legal ) return 0.0 ;
        double v2 = com ( N -> next[1] , is_legal ) ; //计算第二个值
        if ( !is_legal ) return 0.0 ;
        double v3 = com ( N -> next[2] , is_legal ) ; //计算第三个值
        if ( !is_legal ) return 0.0 ;
        double v = ter -> cal ( ter->cal_name , v1 , v2 , v3 , is_legal ); //计算
        return v ;
    }
    else //无法识别结点
    {
        throw_error ( UNKNOWN_NODE_TYPE ) ;
        is_legal = false ;
        return 0.0 ;
    }
}
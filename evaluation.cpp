#include "Node.h"
#include "CG_debug.h"
#include "build_tree.h"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <iostream>

/**
 * Function string2double
 * 作用：将字符串转为double，并判断是否非法或有溢出
 * 传参： v: 要存入的double类型变量 
 *       s：待转换字符串
 * 返回值：一个bool类型变量，表示是否转换成功
**/

bool string2double(double& v, std::string s)
{
    try
    {
        v = stod(s);
    }
    catch(std::invalid_argument&)
    {
        throw_error(ILLEGAL_EXPRESSION);
        return false;
    }
    catch(std::out_of_range&)
    {
        throw_error(DATA_OUT_OF_RANGE);
        return false;
    }
    return true;
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
bool Compute(std::string s, std::map < std::string, Node* >& Var_map,
             std::vector < double > setanswer, double& answer)
{
    std::stringstream in (s);  //将待处理字符串转换为字符串流，读取其中各个字符串
    std::string buf; //从in流中读取字符串第临时变量
    std::vector < std::string > vec; //储存表达式中字符串
    while(in >> buf)
    {
        vec.push_back(buf);     //读取字符串
    }
    if(vec.size() == 0)    //空串，表达式不合法，抛出错误
    {
        throw_error(ILLEGAL_EXPRESSION);
        return false;
    }
    if(vec [ 0 ] == "SETCONSTANT")    //判断表达式类型
    {
        if(vec.size() < 3)     //语法检查：防止不正规表达式
        {
            throw_error(ILLEGAL_EXPRESSION);
            return false;
        }
        if(Var_map.find(vec [ 1 ]) == Var_map.end())  //语法检查：防止计算未出现过的结点
        {
            throw_error(NODE_NOT_FOUND, vec [ 1 ]);
            return false;
        }

        double v;
        if(!string2double(v, vec[2]))  //转为 double，并语法检查
            return false;

        if(Var_map[vec[1]]->get_name() != "Variable")//语法检查：防止SET一个不正确的类型
        {
            throw_error(EVALUATE_NODE_OF_ILLEGAL_TYPE_BY_SET, "SETCONSTANT");
            return false;
        }
        //set操作
        Variable* vc = dynamic_cast < Variable* >(Var_map [ vec [ 1 ] ]);
        vc->eval(v);
        return false;
    }
    else if(vec [ 0 ] == "SETANSWER")    //判断表达式类型
    {
        if(vec.size() < 3)    //语法检查：防止不正规表达式
        {
            throw_error(ILLEGAL_EXPRESSION);
            return false;
        }
        if(Var_map.find(vec [ 1 ]) == Var_map.end())  //语法检查：防止计算未出现过的结点
        {
            throw_error(NODE_NOT_FOUND, vec [ 1 ]);
            return false;
        }

        double v;
        if(!string2double(v, vec[2])) //转为 double，并语法检查
            return false;
        
        if(Var_map [ vec [ 1 ] ]->get_name() != "Variable")   //语法检查：防止SET一个不正确的类型
        {
            throw_error(EVALUATE_NODE_OF_ILLEGAL_TYPE_BY_SET, "SETANSWER");
            return false;
        }
        //为 Variable 赋值
        Variable* vc = dynamic_cast < Variable* >(Var_map [ vec [ 1 ] ]);
        vc->eval(setanswer [ v - 1 ]);
        return false;
    }
    else if(vec [ 0 ] == "EVAL")
    {
        if(Var_map.find(vec [ 1 ]) == Var_map.end())   //语法检查：防止计算一个未出现过的结点
        {
            throw_error(NODE_NOT_FOUND, vec [ 1 ]);
            return false;
        }
        if(vec.size() >= 3)  //确保前三个变量存在
        {
            int Placeholder_number; //要赋值运算符个数
            try//转为 int，并语法检查
            {
                Placeholder_number = stoi(vec [ 2 ]);
            }
            catch(std::invalid_argument&)
            {
                throw_error(PARAMETER_MISSING_IN_EVAL);
                return false;
            }
            catch(std::out_of_range&)
            {
                throw_error(DATA_OUT_OF_RANGE);
                return false;
            }

            if(Placeholder_number * 2 + 3 > vec.size())  //语法检查：防止要赋值运算符个数与实际输入个数不同
            {
                throw_error(ILLEGAL_EXPRESSION);
                return false;
            }
            for(int i = 1; i <= Placeholder_number; i ++)    //给Placeholder结点赋值
            {
                std::string name = vec [ 2 + (2 * i) - 1 ];
                if(Var_map.find(name) == Var_map.end())     //防止赋值未出现过的结点
                {
                    throw_error(NODE_NOT_FOUND, vec [ 2 + (2 * i) - 1 ]);
                    return false;
                }
                Node*& N = Var_map [ name ];
                std::string type_name = N->get_name();
                if(type_name != "Placeholder")    //防止赋值非Placeholder结点
                {
                    throw_error(EVALUATE_NODE_OF_ILLEGAL_TYPE_BY_EVAL);
                    return false;
                }

                double v;
                if(!string2double(v, vec[2 + 2 * i]))  //转为 double，并语法检查
                    return false;

                //赋值操作，之前已检查过 p 为 Placeholder 结点
                Placeholder* p = dynamic_cast < Placeholder* >(N);
                p->eval(v);
            }
        }
        bool is_legal = true; //判定该表达式是否能合法计算
        answer = Var_map[vec[1]]->calculate(is_legal);     //计算

        /**********清除已赋的值**********/

        Var_map[vec[1]]->init();    //递归清除结点

        //此处再执行一次根据输入清除Placeholder结点目的是为了防止表达式给一些
        //该运算不依赖的Placeholder赋值，从而导致Placeholder_Missing.
        //错误不一定触发。
        if(vec.size() >= 3)
        {
            int Placeholder_number = stoi(vec[2]);   //若vec[2]不是能转换成数字的字符串，在赋值时已经跳出函数，不会执行到此处
            //清除赋值
            for(int i = 1; i <= Placeholder_number; i ++)
            {
                std::string name = vec [ 2 + (2 * i) - 1 ];
                Placeholder* p = dynamic_cast < Placeholder* >(Var_map [ name ]);
                p->have_value = false;
            }
        }

        return is_legal; //若该表达式不能合法计算，则在函数内部抛出错误，不需要也无法输出
    }
    throw_error(NO_MATCH_OPERATOR_FOR, vec[0]);    //未找到运算符
    return false;
}


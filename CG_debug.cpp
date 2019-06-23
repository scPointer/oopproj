#include <iostream>
#include <string>

void throw_error(int v, std::string s = "")
{
    switch(v)
    {
    case 0 : //未知错误，如出现一般是代码问题，请检查实现和使用该错误的语句。
    {
        std::cout << "ERROR: Unknown error\n";
        break;
    }
    case 1 : //被零除
    {
        std::cout << "ERROR: Division by zero\n";
        break;
    }
    case 2 : //有依赖结点未赋值
    {
        std::cout << "ERROR: Placeholder missing\n";
        break;
    }
    case 3 : //未定义运算符
    {
        std::cout << "ERROR: No match operator for '" << s << "'\n";
        break;
    }
    case 4 : //LOG函数传入非正数
    {
        std::cout << "ERROR: LOG operator's input must be positive\n";
        break;
    }
    case 6 : //调用eval函数访问非Placeholder结点或非Var结点
    {
        std::cout << "ERROR: Cannot evaluate a non-Placeholder or a non-Var type node\n";
        break;
    }
    case 7 : //试图调用不存在的结点
    {
        std::cout << "ERROR: Cannot find node called '" << s << "'\n";
        break;
    }
    case 10 : //不规范表达式
    {
        std::cout << "ERROR: illegal expression\n";
        break;
    }
    case 12 : //试图用get_value函数访问运算符结点
    {
        std::cout << "ERROR: Type '" << s << "' does not have value\n";
        break;
    }
    case 13 : //调用SETCONSTANT或SETANSWER给一个非Variable结点赋值
    {
        std::cout << "ERROR: Use "<< s << "to evaluate a non-Variable node\n";
        break;
    }
    case 14 : //试图在EVAL语句中给非Placeholder结点赋值
    {
        std::cout << "ERROR: Cannot evaluate a non-Placeholder node\n";
        break;
    }
    case 15 : //未知结点类型
    {
        std::cout << "ERROR: Unknown node type\n";
        break;
    }
    case 16 : //EVAL时第三个参数缺失（赋值Placeholder结点个数）
    {
        std::cout << "ERROR: Please enter the number of Placeholders\n";
        break;
    }
    case 17 : //赋值过大，超过数据类型范围
    {
        std::cout << "ERROR: OUT OF RANGE!\n";
        break;
    }
    case 19 : //重复定义结点（非Var结点）
    {
        std::cout << "ERROR: Node redefined\n";
        break;
    }
    case 20 : //表达式中间的赋值符号
    {
        std::cout << "ERROR: Unnecessary '=' in expressions, did you mean \"==\"?\n";
        break;
    }
    case 21 : //Assert 输入的值为 0
    {
        std::cout << "ERROR: Assertion failed\n";
        break;
    }
    case 22 : //运算符的参数个数错误
    {
        std::cout << "ERROR: The operator COND should have exactly 3 parameters\n";
        std::cout << "And the operator BIND should have exactly 2 parameters\n";
        break;
    }
    case 23 : //试图对常量求导
    {
        std::cout << "ERROR: Trying to get derivative of constant node\n";
        break;
    }
    case 24 : //该操作符不支持求导
    {
        std::cout << "ERROR: Derivative is not available on this operator\n";
        break;
    }
    case 25 : //试图求高阶导数
    {
        std::cout << "ERROR: Higher derivatives are unsupported\n";
        break;
    }
    case 26 : //AT 运算之前没有找到 GRAD 运算
    {
        std::cout << "ERROR: The operator GRAD should be used before AT\n";
        break;
    }
    case 27 : //GRAD 运算之后没有找到 AT 运算
    {
        std::cout << "ERROR: The operator AT should be used after GRAD\n";
        break;
    }
    default : //未知错误类型，请检查参数 v 是否为已定义错误类型
    {
        std::cout << "Unknown error! Please check the function throw_error() got correct parameters\n";
        break;
    }
    }
}

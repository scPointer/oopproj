#include <iostream>
#include <string>

void throw_error ( int v , std::string s = "")
{
    switch ( v )
    {
        case 0 : //一般这个错误是只要不乱搞就不会出现的。如果出现了，那就代表我也不知道哪里出了问题。
        {
            std::cout << "ERROR: Unknown type error\n" ;
            break ;
        }
        case 1 : //被零除
        {
            std::cout << "ERROR: Division by zero\n" ;
            break ;
        }
        case 2 : //有依赖结点未赋值
        {
            std::cout << "ERROR: Placeholder missing\n" ;
            break ;
        }
        case 3 : //未定义运算符
        {
            std::cout << "ERROR: No match operator for '" << s << "'\n" ;
            break ;
        }
        case 4 : //LOG函数传入非正数
        {
            std::cout << "ERROR: LOG operator's input must be positive\n" ;
            break ;
        }
/*        case 5 : //。。。算了我也不知道我定义这个干什么的
 *       {
 *           std::cout << "ERROR: No matching operators\n" ;
 *           break ;
 *       }
 */
        case 6 : //调用eval函数访问非Placeholder结点或非Var结点
        {
            std::cout << "ERROR: Cannot evaluate a non-Placeholder or a non-Var type node\n" ;
            break ;
        }
        case 7 : //试图调用不存在的结点
        {
            std::cout << "ERROR: Cannot find node called '" << s << "'\n" ;
            break ;
        }
        case 8 : //由于未知运算符导致无法计算。其实我觉得这个触发不了。。。
        {
            std::cout << "ERROR: Cannot compute. Please check if you have added new operators. " ;
            break ;
        }
        case 9 : //我为啥要定义这个？？？
        {
            std::cout << "ERROR: Unknown operator type\n" ;
            break ;
        }
        case 10 : //不规范表达式
        {
            std::cout << "ERROR: illegal expression\n" ;
            break ;
        }
        case 11 : //不存在结点（我才发现和7重复了。。。）
        {
            std::cout << "ERROR: Non-existent node '" << s << "'\n" ;
            break ;
        }
        case 12 : //试图用get_value函数访问运算符结点
        {
            std::cout << "ERROR: Type '" << s << "' does not have value\n" ;
            break ;
        }
        case 13 : //调用SETCONSTANT或SETANSWER给一个非Variable结点赋值
        {
            std::cout << "ERROR: Cannot use 'SETCONSTANT' or 'SETANSWER' to evaluate a non-Variable node\n" ;
            break ;
        }
        case 14 : //试图在EVAL语句中给非Placeholder结点赋值
        {
            std::cout << "ERROR: Cannot evaluate a non-Placeholder node\n" ;
            break ;
        }
        case 15 : //未知结点类型
        {
            std::cout << "ERROR: Unknown node type\n" ;
            break ;
        }
        case 16 : //EVAL时第三个参数缺失（赋值Placeholder结点个数）
        {
            std::cout << "ERROR: Please enter the number of Placeholders\n" ;
            break ;
        }
        case 17 : //赋值过大，超过数据类型范围
        {
            std::cout << "ERROR: OUT OF RANGE!\n" ;
            break ;
        }
        case 19 : //重复定义结点（非Var结点）
        {
            std::cout << "ERROR: Node redefined\n" ;
            break ;
        }
        case 20 : //表达式中间的赋值符号
        {
            std::cout << "ERROR: unnecessary '=' in expressions, did you mean \"==\"?\n" ;
            break ;
        }
        case 21 : //Assert 输入的值为 0
        {
            std::cout << "ERROR: Assertion failed\n" ;
            break;
        }
        case 22 : //运算符的参数个数错误
        {
            std::cout << "ERROR: The operator COND should have exactly 3 parameters\n" ;
            std::cout << "And the operator BIND should have exactly 2 parameters\n" ;
            break;
        }
        default : //如果你自己写了throw_error并且随意写参数值。。。那就不知道你想要调用哪个错误类型了
        {
            std::cout << "Unknown type error! Please check if you give function 'throw_error' correct parameters\n" ;
            break ;
        }
    }
}

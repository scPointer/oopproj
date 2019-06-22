#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Node.h"
#include "build_tree.h"
#include "CG_debug.h"
using namespace std;

void build_var ( string s , map < string , Node* >& Var_map ) //第一步赋值
{
    stringstream in ( s ) ;
    vector < string > vec ;
    string buffer ;
    while ( in >> buffer ) vec.push_back ( buffer ) ;
    
    /**********语法检查**********/

    double v ;
    if ( vec.size() >= 3 )
    {
        try
        {
            v = stod ( vec [ 2 ] ) ;
        }
        catch ( std::invalid_argument& )
        {
            throw_error ( ILLEGAL_EXPRESSION ) ;
            return ;
        }
        catch ( std::out_of_range& )
        {
            throw_error ( DATA_OUT_OF_RANGE ) ;
            return ;
        }
    }

    /**********end**********/

    /**********创建Placeholder结点**********/

    if ( vec [ 1 ] == "P" )
    {
        if ( Var_map.find ( vec [ 0 ] ) != Var_map.end() )
        {
            throw_error ( NODE_REDEFINED ) ;
            return ;
        } 
        Node* N = new Placeholder ( vec [ 0 ] ) ;
        Var_map [ vec [ 0 ] ] = N ;
        return ;
    }

    /**********end**********/

    /**********创建Constant结点**********/

    if ( vec [ 1 ] == "C" )
    {
        if ( vec.size () < 3 )
        {
            throw_error ( ILLEGAL_EXPRESSION ) ;
            return ;
        }
        if ( Var_map.find ( vec [ 0 ] ) != Var_map.end() )
        {
            throw_error ( NODE_REDEFINED ) ;
            return ;
        }
        Node* N = new Constant ( vec [ 0 ] , v ) ;
        Var_map [ vec [ 0 ] ] = N ;
        return ;
    }

    /**********end**********/

    /**********创建Variable结点**********/

    if ( vec [ 1 ] == "V" )
    {
        if ( vec.size () < 3 )
        {
            throw_error ( ILLEGAL_EXPRESSION ) ;
            return ;
        }
        if ( Var_map.find ( vec [ 0 ] ) != Var_map.end() ) 
        {
            throw_error ( NODE_REDEFINED ) ;
            return ;
        }
        Node* N = new Variable ( vec [ 0 ] , v ) ;
        Var_map [ vec [ 0 ] ] = N ;
        return ;
    }

    /**********end**********/

    throw_error ( UNKNOWN_NODE_TYPE ) ;
    return ;
}

//创建运算符，如加减乘除之类的运算符
Node* create_calculator(string s, int & count_arg) //后者是此运算符的参数个数
{
    Node* N ;
    if ( s == "PRINT" || s == "SIN" || s == "LOG" || s == "EXP" || s == "SIGMOID" || s == "TANH" || s == "ASSERT")
    {
        N = new Unary_Operator ( s ) ;
        count_arg = 1 ;
    }
    else if(s=="+"||s=="-"||s=="*"||s=="/"||s=="<"||s==">"||s=="<="||s==">="||s=="=="||s=="BIND") //|| "Print" || ...
    {
        //Unary_Operator* una = new Unary_Operator ( s )
        N = new Binary_Operator ( s ) ;
        count_arg = 2 ;
    }
    else if ( s == "COND" )
    {
        N = new Ternary_Operator ( s ) ;
        count_arg = 3 ;
    }
    else if(s == "GRAD")
    {
        N = new Grad_Operator();
        count_arg = 1;
    }
    else if(s == "AT")
    {
        N = new At_Operator();
        count_arg = 2;
    }
    // Binary/...
    else
    {
        puts ( "continue" ) ;
        throw "No match operator" ;
    }
    return N;
}

//优先级
inline int priority ( std::string c )
{
    
    if ( c == "<" ) return 1 ;
    if ( c == ">" ) return 1 ;
    if ( c == "<=" ) return 1 ;
    if ( c == ">=" ) return 1 ;
    if ( c == "==" ) return 1 ;
    if ( c == "+" ) return 2 ;
    if ( c == "-") return 2 ;
    if ( c == "*") return 3 ;
    if ( c == "/") return 3 ;
    if ( c == "AT" ) return 4;
    if ( c == "SIN" ) return 5 ;
    if ( c == "LOG" ) return 5 ;
    if ( c == "EXP" ) return 5 ;
    if ( c == "SIGMOID" ) return 5 ;
    if ( c == "TANH" ) return 5 ;
    if ( c == "PRINT" ) return 6 ;
    if ( c == "ASSERT" ) return 6 ;
    if ( c == "COND" ) return 6 ;
    if ( c == "BIND" ) return 6 ;
    if ( c == "GRAD" ) return 6 ;
    return 0 ;
}

//递归清空y已赋值结点
void init(Node* N)
{
    std::string s = N->get_name();
    if( s == "Var" )
    {
        Var* v = dynamic_cast < Var* > ( N ) ;
        if(v->have_value == false && v->check_grad_empty())
            return;
        v->have_value = false;
        v->clear_grad();
    }
    else if ( s == "Placeholder" )
    {
        Placeholder* p = dynamic_cast < Placeholder* > ( N ) ;
        p->have_value = false ;
    }
    int size = N->next.size();
    for(int i=0;i<size;i++)
    {
        init ( N -> next [ i ] );
    }
}

/**建树函数：预处理
 * 参数：s：一行创建结点的语句
 *      Var_map：结点名称到结点的映射，用于链接结点和判定语法用
**/
void build_tree(string s, std::map < std::string , Node* >& Var_map )   // 要有單純string版本的初始化
//已經確定了第一節為變量名、第二節為 "="
{
    stringstream is(s); //将指令转换为字符串流
    string buf; //读取指令用的临时变量
    vector<string> vec; //储存指令用
    while(is>>buf) vec.push_back(buf); //读取指令
    std::map < std::string , Node* >::iterator iter = Var_map.find ( vec [ 0 ] ) ; //查找链接所需结点
    if ( iter != Var_map.end() && iter -> second -> get_name () != "Var" ) //出现了未定义的结点，报错，退出
    {
        throw_error ( NODE_REDEFINED ) ;
        return ;
    }
    Var* node = new Var(vec[0]); //確定是Var類型
    if ( vec.size () < 2 || vec [ 1 ] != "=" )
    {
        throw_error ( ILLEGAL_EXPRESSION ) ;
        return ;
    }
    bool is_legal = true ;
    Node* N = connect ( vec , Var_map , 2 , vec.size() - 1 , is_legal ) ; //链接
    if ( !is_legal ) //无法链接
    {
        std::cout << "Connect failed\n" ;
        return ;
    }
    node->add_next(N); //将建好的树的根结点（运算符结点）链接到变量结点上
    if(N -> get_name() == "Grad_Operator")
        node -> have_grad_node = true;
    Var_map [ vec [ 0 ] ] = node ; //添加变量结点
}

/**建树：链接结点
 * 参数：vec：输入的指令
 *      Var_map：结点名字到结点的映射，用于链接结点
 *      head、tail：当前处理的语句的位置指示符
 *      is_legal：判定能否正常链接这棵树
 * 返回值：一个Node*结点，表示建好的树的根结点
**/
Node* connect(std::vector<string> vec , std::map<std::string , Node*>& Var_map , int head , int tail , bool& is_legal )
{
    //std::cout << head << " " << tail << "\n" ;
    Node* N;
    if ( head > tail ) //你能触发这个错误算你赢
                       //行吧打脸了，刚写完这个注释我就触发了这个错误。
    {
        is_legal = false ;
        throw_error ( ILLEGAL_EXPRESSION ) ;
        return N ;
    }
    else if(head==tail) //单个操作符
    {
        if ( Var_map.find ( vec [ head ] ) != Var_map.end() )
            N = Var_map [ vec [ head ] ] ;
        else //不存在的结点
        {
            is_legal = false ;
            throw_error ( NODE_NOT_FOUND , vec [ head ] ) ;
            return N ;
        }
    }
    else
    {
        int count_bracket = 0; //判定括号用的参数
        int position_least_priority = -1; //最后一个运算的符号
        int least_priority = 10000;//别搞出10000个运算符就好。。
        int count_arg;//运算符数
        for(int i = head; i<=tail; i++ )
        {
            if(vec[i]=="("){count_bracket++;}
            else if(vec[i]==")"){count_bracket--;}
            else if(priority(vec[i])&&!count_bracket)//有优先度，是运算符，且在括号外面
            {
                int x = priority ( vec [ i ] ) ;
                if(x<=least_priority) //相同优先度就取最右的（左结合）
                {
                    least_priority = x;
                    position_least_priority = i;
                }
            }
        }
        if(position_least_priority<0)//整个式子被括号括起来
        {
            if ( vec [ head ] != "(" || vec [ tail ] != ")" ) //如果没有被括号括起来
            {
                throw_error ( ILLEGAL_EXPRESSION ) ;
                is_legal = false ;
                return N ;
            }
            N = connect(vec, Var_map, head+1, tail-1 , is_legal );
            if ( !is_legal ) return N ;
        }
        else
        {

            /**********语法检查**********/
            //事实上这一步不会出问题，因为position_least_priority不会指向非运算符结点

            try
            {
                N =  create_calculator(vec[position_least_priority], count_arg);//后者会被修改
            }
            catch ( const char* s ) 
            {
                throw_error ( NO_MATCH_OPERATOR_FOR , vec[position_least_priority]) ;
                is_legal = false ;
                return N ;
            }

            /**********语法检查**********/
            switch (count_arg) {
                case 1: //单目运算符
                {
                    Node* n = connect ( vec , Var_map , position_least_priority + 1 , tail , is_legal ) ;
                    if ( !is_legal ) return N ;
                    N->add_next(n);
                    break;
                }
                    
                case 2: //双目运算符
                {
                    if( vec[position_least_priority] == "BIND" ) // 此处默认BIND运算符后接的是两个简单结点。
                    {
                        if(tail != head + 2)
                        {
                            throw_error( PARAMETER_COUNT_ERROR ) ;
                            is_legal = false;
                            return N;
                        }
                        Node* n1 = connect ( vec , Var_map , position_least_priority + 1 , position_least_priority + 1 , is_legal ) ;
                        if ( !is_legal ) return N ;
                        Node* n2 = connect ( vec , Var_map , position_least_priority + 2 , position_least_priority + 2 , is_legal ) ;
                        if ( !is_legal ) return N ;
                        N -> add_next ( n1 ) ; N -> add_next ( n2 ) ;
                        break;
                    }
                    Node* n1 = connect ( vec , Var_map , head , position_least_priority - 1 , is_legal ) ; //链接左侧
                    if ( !is_legal ) return N ;
                    Node* n2 = connect ( vec , Var_map , position_least_priority + 1 , tail , is_legal ) ; //链接右侧
                    if ( !is_legal ) return N ;
                    N -> add_next ( n1 ) ; N -> add_next ( n2 ) ;
                    break;
                }
                
                case 3: //三目运算符
                        //此处默认COND运算符后接的是三个简单结点。
                {
                    if(tail != head + 3)
                    {
                        throw_error( PARAMETER_COUNT_ERROR ) ;
                        is_legal = false;
                        return N;
                    }
                    Node* n1 = connect ( vec , Var_map , position_least_priority + 1 , position_least_priority + 1 , is_legal ) ;
                    if ( !is_legal ) return N ;
                    Node* n2 = connect ( vec , Var_map , position_least_priority + 2 , position_least_priority + 2 , is_legal ) ;
                    if ( !is_legal ) return N ;
                    Node* n3 = connect ( vec , Var_map , position_least_priority + 3 , position_least_priority + 3 , is_legal ) ;
                    if ( !is_legal ) return N ;
                    N -> add_next ( n1 ) ; N -> add_next ( n2 ) ; N -> add_next( n3 );
                    break;
                }
                    
            }
        }
    }
    return N;
}
#ifndef ___COMPUTATIONALGRAPH__NODE_H___
#define ___COMPUTATIONALGRAPH__NODE_H___

#include <vector>
#include <string>

#define eps 1e-6
/* Base: class Node
 * 由于计算图基于树的数据结构，将结点作为基类
 * 变量、常数等所有可被视为节点的结构均基于Node类开发
 * Node类中只需完成树的结构，结点具体作用在继承类里完成
 * 基础内容：next：树的后继结点
 *         del：析构函数主体，删除vecttor:next用
 *         get_name()：返回类的类型
 *         add_next()：添加后继结点
 * 友元：build_tree：建树函数
 *      init：重置结点赋值
 *      com：计算函数
**/

class Node
{
    private:
    std::vector < Node* > next ; //树的后继结点
    protected:
    void del () ; //派生类析构用：删除vector
    friend void build_tree ( std::string exp ) ;
    friend void init ( Node* N ) ;
    friend double com ( Node* N , bool& is_legal ) ;
    public:
    virtual std::string get_name () ;
    void add_next ( Node* N ) ;
    Node () {}
    ~Node () { del() ; }
} ;

/* Derive: class Placeholder
 * 占位符结点：由基类Node继承而来，表示占位符
 * 新增内容：value：结点权值
 *         var_name：结点名称（PRINT使用）
 *         have_value：是否已赋值
 * 友元：get_value：取得该结点的权值
 *      eval：给该节点赋值
 *      get_var_name：获取结点名称
**/ 

class Placeholder : public Node
{
    private:
    double value ;
    std::string var_name ;
    protected:
    friend double get_value ( Node* N ) ;
    friend bool eval ( double v , Node* N ) ;
    friend std::string get_var_name ( Node* N ) ;
    public:
    bool have_value ;
    std::string get_name () ;
    Placeholder ( std::string s , double v = 0.0 ) : var_name  ( s ) , value ( v ) { have_value = false ; }
} ;

/* Derive: class Constant
 * 常数结点：由基类Node继承而来，表示不可改变的常数
 * 新增内容：value：结点权值
 *         var_name：结点名字
 * 友元：get_value：取得该结点权值
 *      get_var_name：取得该结点名字
**/

class Constant : public Node
{
    private:
    double value ;
    std::string var_name ;
    protected:
    friend double get_value ( Node* N ) ;
    friend std::string get_var_name ( Node* N ) ;
    public:
    std::string get_name () ;
    Constant ( std::string s , double v = 0.0 ) :  var_name ( s ) , value ( v ) {}
} ;

/* Derive: class Var
 * 变量结点
 * 新增内容：value：结点权值
 *         var_name：结点名字
 *         have_value：是否已计算过
 * 友元：get_value：取得权值
 *      eval：赋值
 *      get_var_name：取得结点名字
**/

class Var : public Node
{
    private:
    std::string var_name ; //变量名
    double value ; //当前数据下变量的值
    protected:
    friend double get_value ( Node* N ) ;
    friend bool eval ( double v , Node* N ) ;
    friend std::string get_var_name ( Node* N ) ;
    public:
    bool have_value ;
    std::string get_name () ;
    Var ( std::string s , double v = 0.0 ) : var_name ( s ) , value ( v ) { have_value = false ; }
} ;

/* Derive: class Var_Constant
 * 可变常数结点：由基类Node继承而来，表示可以改变数值的常数结点
 * 新增内容：value：权值
 *         var_name：变量名
 *         set：改变结点权值
 * 友元：get_value：取得结点权值
 *      get_var_name：取得结点名字
**/

class Variable : public Node
{
    private:
    double value ;
    std::string var_name ;
    protected:
    friend double get_value ( Node* N ) ;
    friend std::string get_var_name ( Node* N ) ;
    public:
    void set ( double v ) { value = v ; }
    std::string get_name () ;
    Variable ( std::string s , double v ) : var_name ( s ) , value ( v ) {}
} ;

/* Derive: Unary_operator
 * 单目运算符结点
 * 新增内容：cal_name：运算符名称
 *         cal：单目运算符运算函数
 * 友元：com：计算函数
**/

class Unary_Operator : public Node //单目运算符
{
    private:
    std::string cal_name ; //运算符名称
    protected:
    friend double com ( Node* N , bool& is_legal ) ;
    public:
    std::string get_name () ;
    Unary_Operator ( std::string s ) : cal_name ( s ) {}

    double cal ( std::string s , double v , bool& is_legal , std::string print_out = "" ) ;
} ;


/**
 * Derive: Binary_Operator
 * 双目运算符结点
 * 新增内容：cal_name：运算符名称
 *         cal：双目运算符运算函数
 * 友元：com：计算函数
**/
class Binary_Operator : public Node //双目运算符
{
    private:
    std::string cal_name ;
    protected:
    friend double com ( Node* N , bool& is_legal ) ;
    public:
    std::string get_name () ;
    Binary_Operator ( std::string s ) : cal_name ( s ) {}

    double cal ( std::string s , double v1 , double v2 , bool& is_legal ) ;
} ;

/**
 * Derive: Ternary_Operator
 * 三目运算符结点
 * 新增内容：cal_name：运算符名称
 *         cal：三目运算符函数
 * 友元：com：计算函数
**/
class Ternary_Operator : public Node //三目运算符
{
    private:
    std::string cal_name ;
    protected:
    friend double com ( Node* N , bool& is_legal ) ;
    public:
    std::string get_name () ;
    Ternary_Operator ( std::string s ) : cal_name ( s ) {}

    double cal ( std::string s , double v1 , double v2 , double v3 , bool& is_legal ) ;
} ;

#endif
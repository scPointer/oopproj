#ifndef ___COMPUTATIONALGRAPH__NODE_H___
#define ___COMPUTATIONALGRAPH__NODE_H___

#include <vector>
#include <string>
#include <unordered_map>

#define eps 1e-6 // 判断 <=0 时的误差，用于 ASSERT 这类在0附近“突变”的运算符

/* Base: class Node
 * 由于计算图基于树的数据结构，将结点作为基类
 * 变量、常数等所有可被视为节点的结构均基于Node类开发
 * Node类中只需完成树的结构，结点具体作用在继承类里完成
 * 基础内容：next：树的后继结点
 *         get_name()：获取当前类的类型
 *         calculate():计算当前结点
 *         grad():求导
 *         init():清空当前结点信息。这里只负责递归，特化交给子类
 *         add_next()：添加后继结点
**/
class Node
{
protected:
    std::vector < Node* > next; //树的后继结点
public:
    virtual std::string get_name() = 0;
    virtual double calculate(bool& is_legal) = 0;
    virtual double grad(Node* target, bool& is_legal) = 0;
    virtual void init();
    void add_next(Node* N);
    virtual ~Node() {std::vector<Node*>().swap(next);}
};

/* Derive: class ValueNode
 * 值结点：由基类Node继承而来，表示有值的变量或常量结点
 * 新增内容：value：结点权值
 *         var_name：结点名称（PRINT使用）
 *         print_var_name(): 打印结点信息，用于 PRINT 运算
**/

class ValueNode : public Node
{
protected:
    double value;
    std::string var_name;
public:
    void print_var_name();
    ValueNode(std::string s, double v = 0.0) : var_name(s), value(v) {}
};

/* Derive: class Placeholder
 * 占位符结点：继承自ValueNode，表示占位符
 * 新增内容：have_value：结点在当前EVAL语句种是否被访问过
 *         eval ：为结点赋值
 *         其他为重载覆盖基类的方法，见 Node 类定义
**/

class Placeholder : public ValueNode
{
public:
    bool have_value;
    std::string get_name() override;
    double calculate(bool& is_legal) override;
    double grad(Node* target, bool& is_legal) override;
    void init() override;
    void eval(double v);
    Placeholder(std::string s, double v = 0.0) : ValueNode(s, v), have_value(false){}
};

/* Derive: class Constant
 * 常数结点：继承自ValueNode，表示不可改变的常数
 * 新增内容：全为重载覆盖基类的方法，见 Node 类定义
**/

class Constant : public ValueNode
{
public:
    std::string get_name() override;
    double calculate(bool& is_legal) override;
    double grad(Node* target, bool& is_legal) override;
    Constant(std::string s, double v = 0.0) :  ValueNode(s, v) {}
};

/* Derive: class Var
 * 变量结点: 继承自ValueNode,代表中间表达式变量
 * 新增内容：grad_of：存储当前结点已经计算过的导数，避免重复计算
 *         check_grad_empty(): 检查 grad_of 是否为空
 *         have_value：是否已计算过
 *         have_grad_node: 检查下一个结点是否是GRAD运算，用于特判 corner case
 *         compute_at(): 由 AT 结点发出的请求，需要寻找 GRAD 结点
 *         其他为重载覆盖基类的方法，见 Node 类定义
**/

class Var : public ValueNode
{
private:
    std::unordered_map<Node*, double> grad_of;
    bool check_grad_empty();
    void clear_grad();
public:
    bool have_value;
    bool have_grad_node;
    std::string get_name() override;
    double calculate(bool& is_legal) override;
    double grad(Node* target, bool& is_legal) override;
    void init() override;
    double compute_at(Node* target, bool& is_legal);
    Var(std::string s, double v = 0.0) : ValueNode(s, v), 
        have_value(false), have_grad_node(false) {}
    ~Var() override {clear_grad();};
};

/* Derive: class Variable
 * 可变常数结点：继承自ValueNode，表示可以由操作改变数值的结点
 * 新增内容：eval()：给当前结点赋值
 *          其他为重载覆盖基类的方法，见 Node 类定义
**/

class Variable : public ValueNode
{
public:
    void eval(double v) {value = v;};
    std::string get_name() override;
    double calculate(bool& is_legal) override;
    double grad(Node* target, bool& is_legal) override;
    Variable(std::string s, double v) : ValueNode(s, v) {}
};

/* Derive: OperatorNode
 * 运算符结点
 * 新增内容：cal_name：运算符名称
 *         get_priority()：获取运算符优先级
**/

class OperatorNode : public Node
{
protected:
    std::string cal_name;
public:
    static int get_priority(std::string c);
    OperatorNode(std::string s) : cal_name(s) {}
};

/* Derive: Unary_operator
 * 单目运算符结点
 * 新增内容：cal：单目运算符运算函数
 *          其他为重载覆盖基类的方法，见 Node 类定义
**/

class Unary_Operator : public OperatorNode
{
private:
    double cal(double v, bool& is_legal);
public:
    std::string get_name() override;
    double calculate(bool& is_legal) override;
    double grad(Node* target, bool& is_legal) override;
    Unary_Operator(std::string s) : OperatorNode(s) {}
};

/**
 * Derive: Binary_Operator
 * 双目运算符结点
 * 新增内容：cal：双目运算符运算函数
 *          其他为重载覆盖基类的方法，见 Node 类定义
**/

class Binary_Operator : public OperatorNode
{
private:
    double cal(double v1, double v2, bool& is_legal);
public:
    std::string get_name() override;
    double calculate(bool& is_legal) override;
    double grad(Node* target, bool& is_legal) override;
    Binary_Operator(std::string s) : OperatorNode(s) {}

    
};

/**
 * Derive: Ternary_Operator
 * 三目运算符结点
 * 新增内容：cal：三目运算符运算函数
 *          其他为重载覆盖基类的方法，见 Node 类定义
**/

class Ternary_Operator : public OperatorNode
{
private:
    double cal(double v1, double v2, double v3, bool& is_legal);
public:
    std::string get_name() override;
    double calculate(bool& is_legal) override;
    double grad(Node* target, bool& is_legal) override;
    Ternary_Operator(std::string s) : OperatorNode(s) {}
};

/**
 * Derive: Grad_Operator
 * 求梯度运算符结点
 * 新增内容：grad_at：求该结点的后继节点对 target 的偏导数
 *          其他为重载覆盖基类的方法，见 Node 类定义
**/

class Grad_Operator : public Unary_Operator
{  
public:
    Grad_Operator() : Unary_Operator("GRAD") {}
    std::string get_name() override;
    double calculate(bool& is_legal) override;
    double grad(Node* target, bool& is_legal) override;
    double grad_at(Node* target, bool& is_legal);
};

/**
 * Derive: At_Operator
 * 求偏导运算符结点
 * 新增内容：cal：将求值转为求导运算
 *          其他为重载覆盖基类的方法，见 Node 类定义
**/

class At_Operator : public Binary_Operator
{
private:
    double cal(bool& is_legal);
public:
    At_Operator() : Binary_Operator("AT") {}
    std::string get_name() override;
    double calculate(bool& is_legal) override;
    double grad(Node* target, bool& is_legal) override;
};

#endif
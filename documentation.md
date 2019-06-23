# 说明文档
## 特性
- 支持所有类型运算求导
- 建图部分可接受复杂表达式
- 更多的错误处理机制
- 详细见后说明

---
## 计算图库结构
主要由外部接口、结点类架构、错误处理三部分构成。
- 外部接口： 由 `build_tree.cpp/h` 和 `evaluation.cpp/h` 组成，负责将输入文本翻译为图的结构和运算，是用户和图库的中间层。
- 结点类架构：由 `Node.cpp/h` 组成，是计算图的底层核心。从基类 `Node`确定的 `grad()` `calculate()`等等接口往下实现和泛化，所有的运算实际都在结点内部方法中完成。
- 错误处理：由 `CG_debug.cpp/h` 组成，输出错误信息。除题目要求外添加了许多便于调试的错误类型，用于判断上述代码执行中可能遇到的错误。由 `throw_error()` 函数和一些宏定义组成，无类结构。

---
## 接口说明
- 具体用法请参照 `main1.cpp`
- `build_var(string buffer, map<string, Node*>& Var_map, vector<Node*>& all_nodes)` 输入`buffer`中的结点定义信息（例：`x C 1.0`），定义、生成结点并存入 `Var_map`。
- `build_tree(string buffer, map<string, Node*>& Var_map, vector<Node*>& all_nodes)` 输入`buffer`中的一行表达式信息（例：`a = x + y`），定义、生成结点存入 `Var_map`，并连接图中结点。
- `bool Compute(string buffer, map<string, Node*>& Var_map,std::vector<double> setanswer, double& answer)`由 `Var_map`中的计算图计算`buffer`中的一行表达式，并将答案存在`answer`中。
- 上两个函数中的`all_nodes`用于存储产生的所有结点，包括因名字被覆盖导致不在`map`中的无名结点。程序结束后请使用`free_nodes(vector<Node*>& all_nodes)`释放内存。
- 在主函数中需要自行定义上述`Var_map` `setanswer` `all_nodes`用于存储计算图信息。
- **接口设置不是很符合OOP设计规范。我们曾试图将上述接口封装成计算图类，但结构改动太大，涉及上下所有接口，于是暂保持第一阶段的原状不动。**

---
## 结点类架构说明

- 类继承关系
> Node -> ValueNode | OperatorNode
> 
> ValueNode -> Placeholder | Variable | Constant | Var
> 
> OperatorNode -> Unary_Operator | Binary_Operator | Ternary_Operator
> 
> Unary_Operator -> Grad_Operator
> 
> Binary_Operator -> At_Operator


![Image text](.\structure1.png)
- 图片如不够清晰，请在根目录下下载`structure1.png`。
- 当需要计算时，对`EVAL`所要求的结点调用对应方法，结点会执行自己的方法后根据自己的`vector<Node*> next`数组逆向传播，完成计算。`Var`类中的方法会调用它的`have_value`属性，保证了整体计算复杂度。
- 基类`Node`中的`get_name` `calculate` `grad` 三个方法都是纯虚的，这要求每个结点类都有独立的名字，计算和求导实现。这样的设计避免了成吨的 **if/else + dynamic_case** 出现在代码中。
- 结点的运算接口（如求值，求导）基本都对外开放，而结点属性和计算图的连接信息则不对外开放。
- 关于`AT`和`GRAD`的说明：由于这两个运算不同于其他运算“输入若干浮点数，输出一个浮点数或报错”的形式，故从双目/单目运算符中泛化为独立的类。

---
## 错误处理说明
- 如需插入错误信息，在需要报错时调用`void throw_error(int v, std::string s = "")`即可。注意第一个参数请使用`CG_debug.h`中的宏定义，提高可读性。第二个参数为可选错误信息。
- 如需添加新错误信息，在`CG_debug.cpp`中添加新的`case`，并在`CG_debug.h`中添加新的宏定义即可。

---
## 求导特性说明
- 所有运算符支持求导，但不支持高阶导数。**关于特殊运算符求导**：
- - Print：输出原值，返回导数
- - Assert: 判断原值，如为正则返回 0，否则报错。
- - 比较运算符：变为比较导数。
- - BIND：计算两个操作数的原值，计算并返回第一个操作数的导数
- - COND: 判断第一个操作数的原值，返回第二或第三个操作数的导数

---
## 复杂表达式特性说明
- 计算图中的表达式可以是复杂表达式，但一行只能有一个赋值号，且赋值号左侧必须是单个变量。
- 注意每个操作符、括号、变量之间必须空格，否则报错。
- 注意`BIND`和`COND`运算符目前只支持接收单个运算符。它们可以被其他运算嵌套，但不能嵌套其他运算。
- `AT`运算的上一级必须是`GRAD`运算符，例如：
- - `y = ( GRAD ( a + b ) ) AT c` 合法
- - `y = GRAD ( a + b ) AT c` 合法，由运算符优先级实际上和上一行等同
- - `y = ( GRAD a + GRAD b ) AT c ` 不合法

---
## 开发者指导
- 若添加新运算符，请根据传入参数个数，在对应的类的cal函数内加入你的运算，并在 `Node.cpp` 中的 `OperatorNode::get_priority`中添加对应优先级。同时也应修改`build.cpp`中的 connect 函数，create_calculator函数，添加你的运算符，使程序支持你的新运算。  

---
###### 最后，你可以在 https://github.com/scPointer/oopproj 获取本代码
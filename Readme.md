# 编译选项
- -O2优化
- C++14标准
- cmake最低需版本version 3.9

# 运行环境

## Mac OS Mojave
#### 编译指令：
> make main

或
> cmake .  
make

若用cmake编译，则原makefile可能会被覆盖
#### 删除main
> make clean
---
## Ubuntu 17.10 64位
#### 编译指令：
> make main
或
> cmake .  
make

若用cmake编译，则原makefile可能会被覆盖
#### 删除main
> make clean
---
## Windows 10
#### 编译指令：
> make win_main

或
> cmake .  
make

若用cmake编译，则原makefile可能会被覆盖
#### 删除main
> make win_clean

若用cmake编译生成了makefile，则执行
> make clean
---
~~至于其它系统？我们手上只有这些系统，用其他系统的大佬们请自行测试~~

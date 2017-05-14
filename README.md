# 程序设计语言基础 课程大作业

### 运行方式

使用`make`命令编译，或者使用以下命令编译：

`g++ -o main main.cpp Term.cpp -std=c++11 -Wall -g`

然后使用`./main`运行（Windows下将上面命令的`main`改为`main.exe`）

程序会自动读取当前目录下的`input.txt`和`program.txt`并输出到`output.txt`。即使没有输入也需要`input.txt`。

### 结构说明

`main.cpp`为主程序，`Term.cpp`和`Term.h`经过一些修改。

主程序使用递归的方式遍历AST并解释执行。

`run_block`、`run_expr`、`run_boolexpr`分别是解释执行block、expr和boolexpr的函数。

`run_func`是解释执行函数体的函数，会对实参求值并赋值给形参，同时把当前的环境设定为函数闭包指定的活动记录。

`Closure`类定义了函数的闭包，包括函数的代码（指向AST的指针）和活动记录。

`Env`类定义了活动记录，包括局部变量和函数名、access link、返回值信息。`setvar`、`getvar`和`getfunc`用于查找名字（按access link递归向上查找）。

### 测试

`tests`目录中包含了所有的测试代码，`.m`文件是Minilan的源码，`.in`是对应的输入文件，`.out`是期望得到的输出文件。

`test.sh`是用于自动测试的bash脚本，会使用编译好的`main`程序对所有的测试例子进行测试，并与正确结果进行比较。

测试例子中`math.m`测试了所有的算术和布尔运算。`command.m`测试了大部分command用法和功能。`fact.m`和`prime.m`是计算阶乘和素数的实际应用程序，用于测试循环、判断、递归和实际环境中的问题。`function.m`测试了函数的各种定义和调用方法，还有和作用域、递归相关的问题。`scope.m`测试了助教补充说明的作用域问题。


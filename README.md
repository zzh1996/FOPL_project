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

`s_*.m`是助教提供的测试，`counter.m`是书上高阶函数的例子，`func_assign.m`测试了函数作为变量赋值和传递的各种情况，`high_order_func*.m`是要求文档中的例子。gc_*.m是测试垃圾回收的例子。

### 垃圾回收算法

我实现的垃圾回收工作原理如下：

- 每个Block执行结束后触发
- 触发后检测系统时间，每n秒执行一次GC（n可以设定）
- GC时从当前活动记录（`Env`）开始，检测所有可以找到的活动记录，从一个保存所有堆中已经分配内存的活动记录的列表中去掉找到的活动记录，把剩下的删除。

具体实现中，每个Block或者函数调用结束后，执行`gc`函数，把当前的活动记录和其他需要保留的引用作为参数传给`gc`。

活动记录`Env`的构造和析构函数会维护一个当前已经在堆中分配的`Env`指针集合`Env::envs`。

`gc`函数会把`Env::envs`复制一份，调用`remove_env`递归删除所有可以由当前活动记录找到的活动记录，并且把集合中剩下的指针delete掉。

`remove_env`函数会判断当前`Env`指针是否在集合中，如果在，从集合中删除，并且递归调用`remove_env`处理它的`access_link`、`control_link`、所有的（保存了闭包的）变量、函数返回值。这一步相当于在活动记录的引用关系图中执行DFS，把能找到的都从集合删除。

### 垃圾回收结果验证

编写一个大循环，里面会生成闭包、在If和While的块中生成临时变量等等。然后每秒钟GC后输出时间戳和当前堆中`Env`的个数。

关闭GC时：

```
gc_1.m
1496837024      113062
1496837025      534316
1496837026      944470
1496837027      1362002
gc_2.m
1496837028      107915
1496837029      593248
1496837030      1070561
1496837031      1538397
```

可以看到堆中活动记录越来越多。

打开GC时：

```
gc_1.m
1496836980      9
1496836981      10
1496836982      6
1496836983      10
1496836984      9
gc_2.m
1496836985      8
1496836986      11
1496836987      10
1496836988      11
1496836989      8
```

说明GC工作正常，没有内存泄露。（数值变化说明每次GC触发的位置不相同）

长时间运行程序，在Windows任务管理器中可以看到关闭GC时内存一直增长，开启GC时内存占用不变。


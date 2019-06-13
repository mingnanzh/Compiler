# <center><font face="黑体">Lab4实验报告</font>

<center><font color=black size=5 face="楷体">赵铭南 161271033</font></center>



## <font face="黑体">一、完成的功能</font>

-  ### <font face="宋体">目标代码生成：</font>

    &emsp;&emsp;<font color=black size=4 face="宋体">在词法分析、语法分析、语义分析和中间代码生成程序的基础上，将C--源代码翻译为MIPS32指令序列（可以包含伪指令），并在SPIM Simulator上运行。</font>



## <font face="黑体">二、实现方法</font>

- ### <font face="宋体">目标代码生成：</font>

    &emsp;&emsp;在实验一词法语法分析、实验二语义分析以及实验三中间代码生成的基础上，通过对线性中间代码链表进行遍历完成对目标代码的生成。采用的做法另建文件`asmcode.c`来完成目标代码生成的相关操作。

    #### <font face="宋体">目标代码的翻译：</font>

    主要函数：
    ```C
    Intercode translate_program(struct treenode* program);
    void asmcode(Intercode code, FILE* f);
    ```

    &emsp;&emsp;中间代码的翻译过程由`main.c`里`Intercode code = translate_program(root);`开始，通过递归遍历语法树获得整个文件对应的中间代码。最后，调用`asmcode(code, sfile);`通过对线性中间代码链表进行遍历完成对目标代码的生成，并存入文件。

   





## <font face="黑体">三、编译运行方法</font>

- #### <font face="宋体">环境：</font>

    ```
    GNU Linux Release: Ubuntu 18.04, kernel version 4.15.0-47
    GCC version 7.3.0
    GNU Flex 2.6.4
    GNU Bison 3.0.4
    QtSPIM version 9.1.20
    ```

- #### <font face="宋体">编译：</font>

    进入目录`./Code`，终端下输入：`make`，或：

    ```
    flex lexical.l
    bison -d syntax.y
    gcc tree.c syntax.tab.c semantic.c intercode.c asmcode.c main.c -lfl -ly -o parser
    ```

- #### <font face="宋体">运行：</font>

    终端下输入：`./parser [*.cmm] [*.s]`



## <font face="黑体">四、实验总结</font>

&emsp;&emsp;实验四完成了编译器的目标代码生成的功能，能够对C--源代码进行目标代码生成，加深了对编译过程中目标代码生成的理解。
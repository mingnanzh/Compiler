# Complier
编译原理实验
## Lab1
-  ### <font face="宋体">词法分析：</font>
    - <font color=black size=4 face="宋体">能够查出C--源代码中词法未定义的字符以及任何不符合词法单元定义的字符；</font>
    - <font color=black size=4 face="宋体">识别合法的八进制，如035、072；</font>
    - <font color=black size=4 face="宋体">识别合法的十六进制数，如0x23、0X4a；</font>
    - <font color=black size=4 face="宋体">识别合法的指数形式的浮点数，如1.2、.2、1.2e+4；</font>
- ### <font face="宋体">语法分析：</font>
    - <font color=black size=4 face="宋体">能够查出C--源代码中的语法错误；</font>
- ### <font face="宋体">没有词法和语错误的情况，则打印语法树</font>

## Lab2
-  ### <font face="宋体">语义分析：</font>
    <font color=black size=4 face="宋体">可对输入的`*.cmm`文件进行语义分析，并检查如下类型的错误：</font>
    1. <font color=black size=4 face="宋体">错误类型1：变量在使用时未定义。</font>
    2. <font color=black size=4 face="宋体">错误类型2：函数在调用时未经定义。</font>
    3. <font color=black size=4 face="宋体">错误类型3：变量出现重复定义，或变量与前面定义过的结构体名字重复。</font>
    4. <font color=black size=4 face="宋体">错误类型4：函数出现重复定义。</font>
    5. <font color=black size=4 face="宋体">错误类型5：赋值号两边的表达式类型不匹配。</font>
    6. <font color=black size=4 face="宋体">错误类型6：赋值号左边出现一个只有右值的表达式。</font>
    7. <font color=black size=4 face="宋体">错误类型7：操作数类型不匹配或操作数类型与操作符不匹配。</font>
    8. <font color=black size=4 face="宋体">错误类型8：return语句的返回类型与函数定义的返回类型不匹配。</font>
    9. <font color=black size=4 face="宋体">错误类型9：函数调用时实参与形参的数目或类型不匹配。</font>
    10. <font color=black size=4 face="宋体">错误类型10：对非数组型变量使用“[…]”（数组访问）操作符。</font>
    11. <font color=black size=4 face="宋体">错误类型11：对普通变量使用“(…)”或“()”（函数调用）操作符。</font>
    12. <font color=black size=4 face="宋体">错误类型12：数组访问操作符“[…]”中出现非整数。</font>
    13. <font color=black size=4 face="宋体">错误类型13：对非结构体型变量使用“.”操作符。</font>
    14. <font color=black size=4 face="宋体">错误类型14：访问结构体中未定义过的域。</font>
    15. <font color=black size=4 face="宋体">错误类型15：结构体中域名重复定义（同一结构体中），或在定义时对域进行初始化。</font>
    16. <font color=black size=4 face="宋体">错误类型16：结构体的名字与前面定义过的结构体或变量的名字重复。</font>
    17. <font color=black size=4 face="宋体">错误类型17：直接使用未定义过的结构体来定义变量。</font>

## Lab3
-  ### <font face="宋体">中间代码生成：</font>
&emsp;&emsp;<font color=black size=4 face="宋体">在词法分析、语法分析和语义分析的基础上，可对输入的`*.cmm`文件进行中间代码生成。但不支持结构体类型的变量，不支持高维数组的变量以及不支持一维数组作为函数参数出现。</font>




## 在Windows中，我们可以用命令行来解决问题。这里用到一个超级命令 for ##

我来写个范例，大家就知道该怎么处理了。

`      for /R %f in (*.cpp;*.c;*.h) do astyle --style=ansi "%f" `

该命令在当前目录中寻找文件名匹配模式 **.cpp;**.c;**.h 的所有文件(不同模式可用英文逗号隔开)，并且对每个文件%f执行操作：**

> astyle --style=ansi "%f"


FOR /R [[drive:]path] %variable IN (set) DO command [command-parameters]

> 检查以 [drive:]path 为根的目录树，指向每个目录中的
> FOR 语句。如果在 /R 后没有指定目录，则使用当前
> 目录。如果集仅为一个单点(.)字符，则枚举该目录树。

举例：for 命令 遍历 R:\Link 目录指定的文件执行命令 idfonts
```
for /R R:\Link %f in (*.psd;*indd) do idfonts "%f"  

idfonts "R:\Link\12x6  200支  木  轴  标签 .psd"   
idfonts "R:\Link\12x6  200支  纸  轴  标签 .psd"   
idfonts "R:\Link\12x6  200支塑轴标签 .psd"   
idfonts "R:\Link\2009地下长河书法展集.indd"   
idfonts "R:\Link\兰溪精达.indd"   
idfonts "R:\Link\古文竖排模板示例-兰公子.indd"   
```

// 28x12.2
MSYS中文乱码解决<br>
2010-01-12 16:50<br>
Minimal GNU（POSIX）system on Windows，是一个小型的GNU环境，包括基本的bash ，make等等。Windows下最优秀的GNU环境。<br>
具体文件可以在官方网站下载：<br>
<a href='http://sourceforge.net/projects/mingw/files/'>http://sourceforge.net/projects/mingw/files/</a><br>
<br>
--------------------中文乱码解决方案--------------------<br>
#ls显示中文不正常解决方法：<br>
/etc/profile <br>
#添加<br>
alias ls='ls --show-control-chars -F --color=tty'<br>
#输入中文不正常解决方法：<br>
/etc/inputrc.default<br>
~/.inputrc<br>
#更改<br>
set meta-flag on<br>
set input-meta on<br>
set output-meta on<br>
set convert-meta off<br>
---------------------------------------------------------<br>
重新启动msys后中文乱码问题解决<br>
<br>
目前google code 存在 msys-cn 中文化项目<br>
<a href='http://code.google.com/p/msys-cn/'>http://code.google.com/p/msys-cn/</a>
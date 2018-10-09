# VIM代码跳转功能视频
蘭雅sRGB 龙芯小本服务器 | sRGB.vicp.net

![](http://srgb.vicp.net/blog/webp/vim_tags.webp)

### vim中跳转到头文件
- 按 gf 跳转到指定头文件
- 按 Ctrl+o 返回原来文件
- 无法解决: .vimrc中添加 set path==xxx(头文件所在目录)

### vim窗口分割的操作
	:split    水平窗口分割  sp
	:vsplit   垂直窗口分割  vs
	ctrl+w+w  窗口之间进行切换
  
### 多窗口文件操作
	vim -O 1.txt  2.txt 横向并排窗口打开多个文件 -o 纵向
	:sp newfile.txt     新开文件横向并排
	ctrl+w + q(quit), c(close), o(other)  窗口关闭
	ctrl+w + =    窗口等分、微调< > 水平 + - 垂直
  
### vim之函数跳转功能
- 需要安装 ctags
- 生成 tags 索引文件

	ctags -R . 或者 ctags -R *
	ctrl + ] 跳转到函数定义
	ctrl + t 原路返回

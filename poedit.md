Poedit 修改语言包
poedit 下载地址
http://nchc.dl.sourceforge.net/project/poedit/poedit/1.5/poedit-1.5.4-setup.exe
---
装好后 bin 目录下 有这些工具
msgcat.exe
msgfmt.exe
msgmerge.exe
msgunfmt.exe
xgettext.exe
---
反编译 mo 文件成 po 文件，使用 gettext 的 msgunfmt.exe 工具，命令如下
msgunfmt  语言文件.mo  -o 输出文件.po
```
msgunfmt.exe CodeBlocks.mo -o CodeBlocks.po
```
编码 po 文件为 mo 文件，使用 poedit 的 msgfmt.exe 工具，命令如下
```
msgfmt.exe -o d:\test.mo d:\test.po
```

app.preferences.setBooleanPreference('aiFileFormat/PDFCompatibility', false);


/* 设置保存ai文件时默认不勾选“创建PDF兼容文件”选项
方法:
退出AI(如果AI在运行)，文本工具打开 AIPrefs 文件，查找“PDFCompatibility 1”，将此处的1改为0，保存并关闭，重启AI。
也可以通过运行以下简单的脚本，无需重启AI，立即让该设置生效：
app.preferences.setBooleanPreference('aiFileFormat/PDFCompatibility', false);
更多讨论
- See more at: http://applia.tumblr.com/page/13#sthash.2DyBgqLi.dpuf
*/


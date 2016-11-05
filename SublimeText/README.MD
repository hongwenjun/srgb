#  Sublime VC2010编译和运行

  #include <stdio.h>

  int main()
  {
     printf ("VC.sublime-build   vc.cmd \n");
    return 0;
  }


#if(0)
  //  把C++ VC.sublime-build 和 vc.cmd 复制到 SublimeText\\Data\\Packages\\User\n 
  //  修改vc.cmd 中VC编译器实际目录, 如果用空格请加引号

  // VC2010 编译器目录
  C:\CodeBlocks\build\vc

  // Sublime 用户自定义编译系统存放
  C:\soft\SublimeText\Data\Packages\User

  C++ VC.sublime-build
-------------------------------------

{
  "shell_cmd": "$packages\\User\\vc.cmd && cl.exe /nologo \"${file}\" /o \"${file_path}/${file_base_name}\"",
  "file_regex": "^(..[^:]*):([0-9]+):?([0-9]+)?:? (.*)$",
  "working_dir": "${file_path}",
  "selector": "source.c, source.c++",

  "variants": 
  [ 
    {
      "name": "Run",
      "shell_cmd": "$packages\\User\\vc.cmd && cl.exe /nologo \"${file}\" /o \"${file_path}/${file_base_name}\" && \"${file_path}/${file_base_name}\""
    }
    
  ]
}
-------------------------------------

vc.cmd
-------------------------------------

  @echo off
  set PATH=C:\CodeBlocks\build\vc\bin;%PATH%
  set INCLUDE=C:\CodeBlocks\build\vc\include
  set LIB=C:\CodeBlocks\build\vc\lib

-------------------------------------
#endif

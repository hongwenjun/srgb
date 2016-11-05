#  Sublime 使用 VC2010编译器 编译和运行
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



#  Sublime 调用 JAVA SDK 编译和运行

   Java.sublime-build
-------------------------------------
      {
          "cmd": ["javac", "-Xlint", "$file"],
          "file_regex": "^(...*?):([0-9]*):?([0-9]*)",
          "selector": "source.java",


          "variants": [


              { "cmd": ["javac", "-Xlint", "$file"],
                "file_regex": "^(...*?):([0-9]*):?([0-9]*)",
                "selector": "source.java",
                "name": "Java Lintter"
              },  


              { "cmd": ["java", "$file_base_name"],
                "name": "Run Java"
              }
          ]
      }
-------------------------------------
# 设置 CLASSPATH 到电脑环境变量. 
  WIN7 可以设置到用户的环境变量里, <<JAVA编程思想>> 的源码 下载解压 C:\TIJ4\code;
***
*  CLASSPATH 变量设置如下
*  .;..;C:\TIJ4\code;
***

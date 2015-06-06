# CodeBlocks配置MinGW32(GCC) 连接 Mysql数据库操作 #

首先去MySQL官网下载MySQL connector c
## http://dev.mysql.com/downloads/connector/c/ ##
Windows (x86, 32-bit), ZIP Archive		6.0.2	4.8M
(mysql-connector-c-noinstall-6.0.2-win32.zip)


**mysql数据库为  为 NPMserv 中的 MySQL5.1**

NPMserv 是一款图形界面的快速搭建nginx 0.7.63、PHP 5.2.11、MySQL 5.1.35、phpMyAdmin 3.2.3，网站服务器平台的绿色软件。无需安装，具有灵活的移动性(如D:/NPMserv)、打开NPMserv.exe启动服务即可。


在连接数据库之前，先建立一张表。
cmd 开启命令行控制台  执行  D:\NPMserv\MySQL5.1\bin\mysql.exe -uroot -p123456
```
 
查看当前已有的数据库。（SQL语句末尾加上';'表示立即执行当前语句。）
mysql> show databases;

创建数据库
mysql> create database test;

使用数据库（这句不能加分号）
mysql> use test

查看已有的表
mysql> show tables;

创建表
mysql> create table testuser ( id INT, name CHAR(20));

插入数据
mysql> insert into testuser(id, name) values(1001, 'google');
mysql> insert into testuser(id, name) values(1002, 'kingsoft');
mysql> insert into testuser(id, name) values(1003, 'firefox');

```
完成建立数据表后，来做mysql配置
D:\NPMserv\php\libmysql.dll  和  mysql-connector-c-noinstall-6.0.2-win32.zip 中的 libmysql.dll 都可以用
放在可执行文件目录 或者 windows目录下


mysql-connector-c-noinstall-6.0.2-win32.zip  解压开放到 sdk目录，改名为mysql (为了简明)
配置成目录如下，能找到如下文件
# D:\CodeBlocks\sdk\mysql\include\mysql.h #
# D:\CodeBlocks\sdk\mysql\lib\libmysql.lib #

CodeBlocks 建立一个控制台项目，项目名称为test 填入下面源代码
直接编译会找不到头文件和库文件，需要配置下 mysql库

CB中项目管理-->点到项目图标-->右键构建选项--->点到Debug上一级tset项目名称，使配置对整个项目有效
连接器设置-->链接库里添加  D:\CodeBlocks\sdk\mysql\lib\libmysql.lib  (因为是导入库，Mingw也可以用VC的导入库)
搜索路径-->编译器里添加  D:\CodeBlocks\sdk\mysql\include ; -->链接器路径可以不选，因为上面 libmysql.lib为绝对路径

完成配置后 可以编程链接成 test.exe ，如果提示找不到 libmysql.dll，请复制到 windows 下
如果 test.exe 应用程序错误  "0xaaaaaaaa"指令引用的"0x00000000c" 内存。改内存不能为读"read"
请检查代码的查询语句是否正确。如果数据表按上面生成。这个两个查询语句都可以用
# mysql\_query(ssock, "select **from testuser") #
# mysql\_query(ssock, "select** from testuser where id < 1002") #

```
#include <stdio.h>
#include <winsock.h>
#include "mysql.h"


int main(int argc, char* argv[])
{
    unsigned short Port = 3306;
    char* IPAddress = "localhost";
    char* UserName = "root";
    char* Password = "123456";
    char* DBName = "test";
    unsigned long i;
    printf("Start... \n");

    MYSQL* ssock;
    MYSQL_RES*   res;
    MYSQL_ROW   row;
    ssock = (MYSQL*)malloc(sizeof(MYSQL));
    mysql_init(ssock);
    if (ssock == NULL) {
        printf("EROR: MySQL ssock init error. \n");
        return FALSE;
    }
    printf("MySQL ssock init OK. \n");

    //连接到指定的数据库
    ssock = mysql_real_connect(ssock, IPAddress, UserName, Password, NULL, Port, NULL, 0);
    if (!ssock) {
        printf("conn fail... \n");
        unsigned int mtint = mysql_errno(ssock);
        return FALSE;
    }
    printf("MySQL connnect OK... \n");

    if (mysql_select_db(ssock, DBName) != 0) {
        printf("select db error. \n");
        return FALSE;
    }
    printf("select db OK. \n");
    printf("version=%d \n", mysql_get_server_version(ssock));
    //SQL查询语句
    if (mysql_query(ssock, "select * from testuser where id")) {
        printf("mysql_query() Error, %s\n", mysql_error(ssock));
    }

    if (!(res = mysql_store_result(ssock))) {
        printf("mysql_store_result() Error, %s\n", mysql_error(ssock));
    }

    while ((row = mysql_fetch_row(res))) {
        for (i = 0 ; i < mysql_num_fields(res); i++) {
            printf("%s ", row[i]);
        }
        printf("\n");
    }
    mysql_free_result(res);
    mysql_close(ssock);
    printf("End... \n");
    return TRUE;
}
```
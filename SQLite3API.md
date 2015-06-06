# SQLite3 C/C++ 开发接口简介（API函数) #

### 1.0 总览 ###

SQLite3是SQLite一个全新的版本,它虽然是在SQLite 2.8.13的代码基础之上开发的,但是使用了和之前的版本不兼容的数据库格式和API. SQLite3是为了满足以下的需求而开发的:

支持UTF-16编码.
用户自定义的文本排序方法.
可以对BLOBs字段建立索引.
因此为了支持这些特性我改变了数据库的格式,建立了一个与之前版本不兼容的3.0版. 至于其他的兼容性的改变,例如全新的API等等,都将在理论介绍之后向你说明,这样可以使你最快的一次性摆脱兼容性问题.

3.0版的和2.X版的API非常相似,但是有一些重要的改变需要注意. 所有API接口函数和数据结构的前缀都由"sqlite_"改为了"sqlite3_". 这是为了避免同时使用SQLite 2.X和SQLite 3.0这两个版本的时候发生链接冲突.

由于对于C语言应该用什么数据类型来存放UTF-16编码的字符串并没有一致的规范. 因此SQLite使用了普通的void**类型来指向UTF-16编码的字符串. 客户端使用过程中可以把void\*映射成适合他们的系统的任何数据类型.**

### 2.0 C/C++ 接口 ###

SQLite 3.0一共有83个API函数,此外还有一些数据结构和预定义(#defines). (完整的API介绍请参看另一份文档.) 不过你们可以放心,这些接口使用起来不会像它的数量所暗示的那么复杂. 最简单的程序仍然使用三个函数就可以完成: sqlite3\_open(), sqlite3\_exec(), 和 sqlite3\_close(). 要是想更好的控制数据库引擎的执行,可以使用提供的sqlite3\_prepare()函数把SQL语句编译成字节码,然后在使用sqlite3\_step()函数来执行编译后的字节码. 以sqlite3\_column\_开头的一组API函数用来获取查询结果集中的信息. 许多接口函数都是成对出现的,同时有UTF-8和UTF-16两个版本. 并且提供了一组函数用来执行用户自定义的SQL函数和文本排序函数.

2.1 如何打开关闭数据库
```
   typedef struct sqlite3 sqlite3;
   int sqlite3_open(const char*, sqlite3**);
   int sqlite3_open16(const void*, sqlite3**);
   int sqlite3_close(sqlite3*);
   const char *sqlite3_errmsg(sqlite3*);
   const void *sqlite3_errmsg16(sqlite3*);
   int sqlite3_errcode(sqlite3*);
```
sqlite3\_open() 函数返回一个整数错误代码,而不是像第二版中一样返回一个指向sqlite3结构体的指针. sqlite3\_open() 和 sqlite3\_open16() 的不同之处在于sqlite3\_open16() 使用UTF-16编码(使用本地主机字节顺序)传递数据库文件名. 如果要创建新数据库, sqlite3\_open16() 将内部文本转换为UTF-16编码, 反之sqlite3\_open() 将文本转换为UTF-8编码.

打开或者创建数据库的命令会被缓存,直到这个数据库真正被调用的时候才会被执行. 而且允许使用PRAGMA声明来设置如本地文本编码或默认内存页面大小等选项和参数.

sqlite3\_errcode() 通常用来获取最近调用的API接口返回的错误代码. sqlite3\_errmsg() 则用来得到这些错误代码所对应的文字说明. 这些错误信息将以 UTF-8 的编码返回,并且在下一次调用任何SQLite API函数的时候被清除. sqlite3\_errmsg16() 和 sqlite3\_errmsg() 大体上相同,除了返回的错误信息将以 UTF-16 本机字节顺序编码.

SQLite3的错误代码相比SQLite2没有任何的改变,它们分别是:
```
#define SQLITE_OK           0   /* Successful result */
#define SQLITE_ERROR        1   /* SQL error or missing database */
#define SQLITE_INTERNAL     2   /* An internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite_interrupt() */
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* (Internal Only) Table or record not found */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* Database lock protocol error */
#define SQLITE_EMPTY       16   /* (Internal Only) Database table is empty */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* Too much data for one row of a table */
#define SQLITE_CONSTRAINT  19   /* Abort due to contraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_ROW         100  /* sqlite_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite_step() has finished executing */
```

2.2 执行 SQL 语句
```
       typedef int (*sqlite_callback)(void*,int,char**, char**);
       int sqlite3_exec(sqlite3*, const char *sql, sqlite_callback, void*, char**);
```
sqlite3\_exec 函数依然像它在SQLite2中一样承担着很多的工作. 该函数的第二个参数中可以编译和执行零个或多个SQL语句. 查询的结果返回给回调函数. 更多地信息可以查看API 参考.

在SQLite3里,sqlite3\_exec一般是被准备SQL语句接口封装起来使用的.
```
       typedef struct sqlite3_stmt sqlite3_stmt;
       int sqlite3_prepare(sqlite3*, const char*, int, sqlite3_stmt**, const char**);
       int sqlite3_prepare16(sqlite3*, const void*, int, sqlite3_stmt**, const void**);
       int sqlite3_finalize(sqlite3_stmt*);
       int sqlite3_reset(sqlite3_stmt*);
```
sqlite3\_prepare 接口把一条SQL语句编译成字节码留给后面的执行函数. 使用该接口访问数据库是当前比较好的的一种方法.

sqlite3\_prepare() 处理的SQL语句应该是UTF-8编码的. 而sqlite3\_prepare16() 则要求是UTF-16编码的. 输入的参数中只有第一个SQL语句会被编译. 第四个参数则用来指向输入参数中下一个需要编译的SQL语句存放的SQLite statement对象的指针, 任何时候如果调用 sqlite3\_finalize() 将销毁一个准备好的SQL声明. 在数据库关闭之前，所有准备好的声明都必须被释放销毁. sqlite3\_reset() 函数用来重置一个SQL声明的状态，使得它可以被再次执行.

SQL声明可以包含一些型如"?" 或 "?nnn" 或 ":aaa"的标记， 其中"nnn" 是一个整数，"aaa" 是一个字符串. 这些标记代表一些不确定的字符值（或者说是通配符），可以在后面用sqlite3\_bind 接口来填充这些值. 每一个通配符都被分配了一个编号（由它在SQL声明中的位置决定，从1开始），此外也可以用 "nnn" 来表示 "?nnn" 这种情况. 允许相同的通配符在同一个SQL声明中出现多次, 在这种情况下所有相同的通配符都会被替换成相同的值. 没有被绑定的通配符将自动取NULL值.
```
       int sqlite3_bind_blob(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
       int sqlite3_bind_double(sqlite3_stmt*, int, double);
       int sqlite3_bind_int(sqlite3_stmt*, int, int);
       int sqlite3_bind_int64(sqlite3_stmt*, int, long long int);
       int sqlite3_bind_null(sqlite3_stmt*, int);
       int sqlite3_bind_text(sqlite3_stmt*, int, const char*, int n, void(*)(void*));
       int sqlite3_bind_text16(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
       int sqlite3_bind_value(sqlite3_stmt*, int, const sqlite3_value*);
```
以上是 sqlite3\_bind 所包含的全部接口，它们是用来给SQL声明中的通配符赋值的. 没有绑定的通配符则被认为是空值. 绑定上的值不会被sqlite3\_reset()函数重置. 但是在调用了sqlite3\_reset()之后所有的通配符都可以被重新赋值.

在SQL声明准备好之后(其中绑定的步骤是可选的), 需要调用以下的方法来执行:

> int sqlite3\_step(sqlite3\_stmt**);**

如果SQL返回了一个单行结果集，sqlite3\_step() 函数将返回 SQLITE\_ROW , 如果SQL语句执行成功或者正常将返回 SQLITE\_DONE , 否则将返回错误代码. 如果不能打开数据库文件则会返回 SQLITE\_BUSY . 如果函数的返回值是 SQLITE\_ROW, 那么下边的这些方法可以用来获得记录集行中的数据:
```
       const void *sqlite3_column_blob(sqlite3_stmt*, int iCol);
       int sqlite3_column_bytes(sqlite3_stmt*, int iCol);
       int sqlite3_column_bytes16(sqlite3_stmt*, int iCol);
       int sqlite3_column_count(sqlite3_stmt*);
       const char *sqlite3_column_decltype(sqlite3_stmt *, int iCol);
       const void *sqlite3_column_decltype16(sqlite3_stmt *, int iCol);
       double sqlite3_column_double(sqlite3_stmt*, int iCol);
       int sqlite3_column_int(sqlite3_stmt*, int iCol);
       long long int sqlite3_column_int64(sqlite3_stmt*, int iCol);
       const char *sqlite3_column_name(sqlite3_stmt*, int iCol);
       const void *sqlite3_column_name16(sqlite3_stmt*, int iCol);
       const unsigned char *sqlite3_column_text(sqlite3_stmt*, int iCol);
       const void *sqlite3_column_text16(sqlite3_stmt*, int iCol);
       int sqlite3_column_type(sqlite3_stmt*, int iCol);
```
sqlite3\_column\_count()函数返回结果集中包含的列数. sqlite3\_column\_count() 可以在执行了 sqlite3\_prepare()之后的任何时刻调用. sqlite3\_data\_count()除了必需要在sqlite3\_step()之后调用之外，其他跟sqlite3\_column\_count() 大同小异. 如果调用sqlite3\_step() 返回值是 SQLITE\_DONE 或者一个错误代码, 则此时调用sqlite3\_data\_count() 将返回 0 ，然而 sqlite3\_column\_count() 仍然会返回结果集中包含的列数.

返回的记录集通过使用其它的几个 sqlite3\_column**_() 函数来提取, 所有的这些函数都把列的编号作为第二个参数. 列编号从左到右以零起始. 请注意它和之前那些从1起始的参数的不同._

sqlite3\_column\_type()函数返回第N列的值的数据类型. 具体的返回值如下:**

  1. efine SQLITE\_INTEGER  1
  1. efine SQLITE\_FLOAT    2
  1. efine SQLITE\_TEXT     3
  1. efine SQLITE\_BLOB     4
  1. efine SQLITE\_NULL     5

sqlite3\_column\_decltype() 则用来返回该列在 CREATE TABLE 语句中声明的类型. 它可以用在当返回类型是空字符串的时候. sqlite3\_column\_name() 返回第N列的字段名. sqlite3\_column\_bytes() 用来返回 UTF-8 编码的BLOBs列的字节数或者TEXT字符串的字节数. sqlite3\_column\_bytes16() 对于BLOBs列返回同样的结果，但是对于TEXT字符串则按 UTF-16 的编码来计算字节数. sqlite3\_column\_blob() 返回 BLOB 数据. sqlite3\_column\_text() 返回 UTF-8 编码的 TEXT 数据. sqlite3\_column\_text16() 返回 UTF-16 编码的 TEXT 数据. sqlite3\_column\_int() 以本地主机的整数格式返回一个整数值. sqlite3\_column\_int64() 返回一个64位的整数. 最后, sqlite3\_column\_double() 返回浮点数.

不一定非要按照sqlite3\_column\_type()接口返回的数据类型来获取数据. 数据类型不同时软件将自动转换.

2.3 用户自定义函数
可以使用以下的方法来创建用户自定义的SQL函数:
```
   typedef struct sqlite3_value sqlite3_value;
   int sqlite3_create_function(
     sqlite3 *,
     const char *zFunctionName,
     int nArg,
     int eTextRep,
     void*,
     void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
     void (*xStep)(sqlite3_context*,int,sqlite3_value**),
     void (*xFinal)(sqlite3_context*)
   );
   int sqlite3_create_function16(
     sqlite3*,
     const void *zFunctionName,
     int nArg,
     int eTextRep,
     void*,
     void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
     void (*xStep)(sqlite3_context*,int,sqlite3_value**),
     void (*xFinal)(sqlite3_context*)
   );
   #define SQLITE_UTF8     1
   #define SQLITE_UTF16    2
   #define SQLITE_UTF16BE  3
   #define SQLITE_UTF16LE  4
   #define SQLITE_ANY      5
```
nArg 参数用来表明自定义函数的参数个数. 如果参数值为0，则表示接受任意个数的参数. 用 eTextRep 参数来表明传入参数的编码形式. 参数值可以是上面的五种预定义值. SQLite3 允许同一个自定义函数有多种不同的编码参数的版本. 数据库引擎会自动选择转换参数编码个数最少的版本使用.

普通的函数只需要设置 xFunc 参数，而把 xStep 和 xFinal 设为NULL. 聚合函数则需要设置 xStep 和 xFinal 参数，然后把 xFunc 设为NULL. 该方法和使用sqlite3\_create\_aggregate() API一样.

sqlite3\_create\_function16()和sqlite\_create\_function()的不同就在于自定义的函数名一个要求是 UTF-16 编码，而另一个则要求是 UTF-8.

请注意自定函数的参数目前使用了sqlite3\_value结构体指针替代了SQLite version 2.X中的字符串指针. 下面的函数用来从sqlite3\_value结构体中提取数据:
```
   const void *sqlite3_value_blob(sqlite3_value*);
   int sqlite3_value_bytes(sqlite3_value*);
   int sqlite3_value_bytes16(sqlite3_value*);
   double sqlite3_value_double(sqlite3_value*);
   int sqlite3_value_int(sqlite3_value*);
   long long int sqlite3_value_int64(sqlite3_value*);
   const unsigned char *sqlite3_value_text(sqlite3_value*);
   const void *sqlite3_value_text16(sqlite3_value*);
   int sqlite3_value_type(sqlite3_value*);

// 上面的函数调用以下的API来获得上下文内容和返回结果:

   void *sqlite3_aggregate_context(sqlite3_context*, int nbyte);
   void *sqlite3_user_data(sqlite3_context*);
   void sqlite3_result_blob(sqlite3_context*, const void*, int n, void(*)(void*));
   void sqlite3_result_double(sqlite3_context*, double);
   void sqlite3_result_error(sqlite3_context*, const char*, int);
   void sqlite3_result_error16(sqlite3_context*, const void*, int);
   void sqlite3_result_int(sqlite3_context*, int);
   void sqlite3_result_int64(sqlite3_context*, long long int);
   void sqlite3_result_null(sqlite3_context*);
   void sqlite3_result_text(sqlite3_context*, const char*, int n, void(*)(void*));
   void sqlite3_result_text16(sqlite3_context*, const void*, int n, void(*)(void*));
   void sqlite3_result_value(sqlite3_context*, sqlite3_value*);
   void *sqlite3_get_auxdata(sqlite3_context*, int);
   void sqlite3_set_auxdata(sqlite3_context*, int, void*, void (*)(void*));
```
2.4 用户自定义排序规则
下面的函数用来实现用户自定义的排序规则:
```
   sqlite3_create_collation(sqlite3*, const char *zName, int eTextRep, void*,
      int(*xCompare)(void*,int,const void*,int,const void*));
   sqlite3_create_collation16(sqlite3*, const void *zName, int eTextRep, void*,
      int(*xCompare)(void*,int,const void*,int,const void*));
   sqlite3_collation_needed(sqlite3*, void*, 
      void(*)(void*,sqlite3*,int eTextRep,const char*));
   sqlite3_collation_needed16(sqlite3*, void*,
      void(*)(void*,sqlite3*,int eTextRep,const void*));
```
sqlite3\_create\_collation() 函数用来声明一个排序序列和实现它的比较函数. 比较函数只能用来做文本的比较. eTextRep 参数可以取如下的预定义值 SQLITE\_UTF8, SQLITE\_UTF16LE, SQLITE\_UTF16BE, SQLITE\_ANY，用来表示比较函数所处理的文本的编码方式. 同一个自定义的排序规则的同一个比较函数可以有 UTF-8, UTF-16LE 和 UTF-16BE 等多个编码的版本. sqlite3\_create\_collation16()和sqlite3\_create\_collation() 的区别也仅仅在于排序名称的编码是 UTF-16 还是 UTF-8.

可以使用 sqlite3\_collation\_needed() 函数来注册一个回调函数，当数据库引擎遇到未知的排序规则时会自动调用该函数. 在回调函数中可以查找一个相似的比较函数，并激活相应的sqlite\_3\_create\_collation()函数. 回调函数的第四个参数是排序规则的名称，同样sqlite3\_collation\_needed采用 UTF-8 编码. sqlite3\_collation\_need16() 采用 UTF-16 编码.
/*****************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * $Id: simple.c,v 1.6 2004-08-23 14:22:52 bagder Exp $
 */

int main(void)
{
  CURL *curl;
  CURLcode res;
    FILE * pFile;
  pFile = fopen ("myfile.txt","w");

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "curl.haxx.se");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, pFile);
    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}

#if(0)

//  getinfo.c  编译: gcc -lcurl  getinfo.c
#include <stdio.h>
#include <curl/curl.h>

int main(int argc, char* argv[])
{
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
    res = curl_easy_perform(curl);

    if(CURLE_OK == res) {
      char *ct;
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);

      if((CURLE_OK == res) && ct)
        printf("We received Content-Type: %s\n", ct);
    }
    curl_easy_cleanup(curl);
  }
  return 0;
}


libcurl中curl_easy_setopt()和curl_easy_perform()函数使用中文详解
这篇博文介绍的setopt函数不尽详细，改天有空自己搞个译文，然后顺理成章的摘掉转的帽子

----------------------------------------------------------------------------------
2012-02-11 21:14
差不多整理了一遍，又补充了一些，最近正在用libcurl，就用到哪补充到哪吧。顺道摘了转的帽子先
-------------------------------------------------------------------------------

libcurl里主要用到的函数有curl_easy_init(),curl_easy_setopt()和curl_easy_perform().
顾名思义，curl_easy_init()是用来初始化libcurl库的（不知道对不对，就这么理解吧），
curl_easy_setopt()是用来设置各种curl参数的，setopt就是set option，
curl_easy_perform()是用来执行curl的各种操作的，并且通过分析其返回值，可以判断curl执行的情况。
1）curl_easy_setopt函数介绍

本节主要介绍curl_easy_setopt中跟http相关的参数。注意本节的阐述都是以libcurl作为主体，其它为客体来阐述的。
1.CURLOPT_URL
设置访问URL
2.CURLOPT_WRITEFUNCTION，CURLOPT_WRITEDATA
通过设定一个回调函数，执行libcurl在接受到数据后用户想进行的操作，通常函数多做数据保存的功能，如处理下载文件。CURLOPT_WRITEDATA 用于表明CURLOPT_WRITEFUNCTION函数中的stream指针的来源，说白了就是设定回调函数的第四个参数的数据类型。回调函数原型为：size_t function( void *ptr, size_t size, size_t nmemb, void *stream);
3.CURLOPT_HEADERFUNCTION，CURLOPT_HEADERDATA
通过设定一个回调函数，执行libcurl在接受到http头数据后用户想进行的操作。CURLOPT_WRITEDATA 传递指针给libcurl，该指针表明CURLOPT_HEADERFUNCTION 函数的stream指针的来源。回调函数原型为 size_t function( void *ptr, size_t size,size_t nmemb, void *stream);
4.CURLOPT_READFUNCTION CURLOPT_READDATA
libCurl需要读取数据传递给远程主机时将调用CURLOPT_READFUNCTION指定的函数，函数原型是：size_t function(void *ptr, size_t size, size_t nmemb,void *stream). CURLOPT_READDATA 表明CURLOPT_READFUNCTION函数原型中的stream指针来源，说白了就是设定回调函数的第四个参数的数据类型。
5.CURLOPT_NOPROGRESS，CURLOPT_PROGRESSFUNCTION，CURLOPT_PROGRESSDATA
跟数据传输进度相关的参数。CURLOPT_PROGRESSFUNCTION 指定的函数正常情况下每秒被libcurl调用一次，为了使CURLOPT_PROGRESSFUNCTION被调 用，CURLOPT_NOPROGRESS必须被设置为false，CURLOPT_PROGRESSDATA指定的参数将作为 CURLOPT_PROGRESSFUNCTION指定函数的第一个参数
6.CURLOPT_TIMEOUT，CURLOPT_CONNECTIONTIMEOUT:
CURLOPT_TIMEOUT 用于设置传输时间，CURLOPT_CONNECTIONTIMEOUT 设置连接等待时间
7.CURLOPT_FOLLOWLOCATION
设置重定位URL
CURLOPT_RANGE: CURLOPT_RESUME_FROM:
断点续传相关设置。CURLOPT_RANGE 指定char *参数传递给libcurl，用于指明http域的RANGE头域，例如：
表示头500个字节：bytes=0-499
表示第二个500字节：bytes=500-999
表示最后500个字节：bytes=-500
表示500字节以后的范围：bytes=500-
第一个和最后一个字节：bytes=0-0,-1
同时指定几个范围：bytes=500-600,601-999
    CURLOPT_RESUME_FROM 传递一个long参数给libcurl，指定你希望开始传递的偏移量。
8.CURLOPT_UPLOAD：
如果第三个参数被设置为1的话，就是让libcurl做好上传的准备。如果传输协议是http的话，uoload就是发送put。
9. CURLOPT_SSL_VERIFYPEER:
第三个参数的缺省值为1.该函数多用于设定curl忽略对网站证书的检查（不管忽略不忽略，curl都是检查的）。
10.CURLOPT_VERBOSE
相当厉害的一个参数，可以向控制台（默认）输出curl接受和发送的数据，输出流可以重定向。
11.CURLOPT_HTTPGET
将curl向服务器交互数据的方式改变为get

2）curl_easy_perform 函数说明（error 状态码）

该函数完成curl_easy_setopt指定的所有选项，本节重点介绍curl_easy_perform的返回值。返回0意味一切ok，非0代表错误发生。主要错误码说明：
1.CURLE_OK
任务完成一切都好
2.CURLE_UNSUPPORTED_PROTOCOL
不支持的协议，由URL的头部指定
3.CURLE_COULDNT_CONNECT
不能连接到remote 主机或者代理
4.CURLE_REMOTE_ACCESS_DENIED
访问被拒绝
5.CURLE_HTTP_RETURNED_ERROR
Http返回错误
6.CURLE_READ_ERROR
读本地文件错误

3）curl_easy_getinfo函数说明

1.CURLINFO_RESPONSE_CODE
获得http返回的状态码，如400，200
#endif

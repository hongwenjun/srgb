/* localtime example */
#include <stdio.h>      /* puts, printf */
#include <time.h>       /* time_t, struct tm, time, localtime */
 
// 按asctime改写的
char* china_time(const struct tm* timeptr)
{
    static const char wday_name[][8] = {
        "星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"
    };
 
    static const char mon_name[][8] = {
        "一月", "二月", "三月", "四月", "五月", "六月",
        "七月", "八月", "九月", "十月", "十一月", "十二月"
    };
 
    static char result[32];
    sprintf(result, "%d年%02d月%02d日 %s %.2d:%.2d:%.2d\n",
            1900 + timeptr->tm_year,
            timeptr->tm_mon + 1,     // mon_name[timeptr->tm_mon],
            timeptr->tm_mday,
            wday_name[timeptr->tm_wday],
            timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec)
    ;
    return result;
}
 
 
 
int main()
{
    time_t rawtime;
    struct tm* timeinfo;
 
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Current local time and date: %s", asctime(timeinfo));
    printf("中国北京时间: %s", china_time(timeinfo));
 
    return 0;
}
 
 
 
#if(0)
 
 
//#ifndef _TIME_T_DEFINED  /* Also in time.h */
typedef long time_t;
//#define _TIME_T_DEFINED
//
//
//#ifndef _TM_DEFINED /* Also in time.h */
struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };
//#define _TM_DEFINED
 
 
time_t time(time_t* time);
功能： 函数返回当前时间，如果发生错误返回零。如果给定参数time ，那么当前时间存储到参数time中。
 
 
struct tm* localtime(const time_t* time);
 
 
功能：函数返回本地日历时间
 
 
 
char* asctime(const struct tm *timeptr)
{
  static const char wday_name[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char mon_name[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  static char result[26];
  sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
    wday_name[timeptr->tm_wday],
    mon_name[timeptr->tm_mon],
    timeptr->tm_mday, timeptr->tm_hour,
    timeptr->tm_min, timeptr->tm_sec,
    1900 + timeptr->tm_year);
  return result;
}
 
 
#endif

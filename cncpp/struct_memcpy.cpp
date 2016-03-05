#include <stdio.h>
#include <string.h>

struct Date {
    int Day;
    int Month;
    int Year;
};



int main()
{
    Date today = { 5, 3, 2016    };
    Date*  ps_day =  new Date;

    // 结构的复制，使用 memcpy复制
    memcpy(ps_day, &today, sizeof(Date));
    printf("%d-%d-%d\n",  ps_day->Year, ps_day->Month, ps_day->Day);

    ps_day->Year = 2018;
    ps_day->Month += 2;
    memcpy(&today, ps_day,  sizeof(today));
    printf("%d-%d-%d\n",  today.Year, today.Month, today.Day);

    delete ps_day;
    return 0;
}

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

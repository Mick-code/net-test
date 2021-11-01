#include "mysqldb.h"

/* 错误检查 */
static inline void _mysql_check(MYSQL *mysql)
{
    fprintf(stderr, "%s\n", mysql_error(mysql));
    mysql_close(mysql);
    exit(EXIT_FAILURE);
}

/* 连接mysql */
void mysqldb_connect(MYSQL *mysql)
{
    if (!mysql_real_connect(mysql, _STR_MHOST, _STR_MUSER, _STR_MPASSWD, _STR_MDB, 0, NULL, 0))
    {
        printf("\nFailed to connect:%s\n", mysql_error(mysql));
    }
    else
    {
        printf("\nConnect sucessfully!\n");
    }
}

/* 插入数据 */
void mysqldb_insert(MYSQL *mysql, char *field_name, char *message)
{
    int t;
    char query[70];
    char *head = "INSERT INTO ";
    char *left = "(";
    char *right = ") ";
    char *values = "VALUES";
    sprintf(query, "%s%s%s%s%s%s%s%s%s", head, _STR_MTB, left, field_name, right, values, left, message, right);
    printf("%s\n", query);
    mysql_real_query(mysql, query, strlen(query));
}

/* 登入 */
void mysqldb_login(MYSQL *mysql, int fd, char *name)
{
    char field_name[] = "fd, name\0";
    char message[50] = {};
    sprintf(message, "%d,\"%s\"", fd, name);
    mysqldb_insert(mysql, field_name, message);
}

/* 删除数据 */
void mysqldb_delete(MYSQL *mysql, char *field_name, int key)
{
    char *head = "DELETE FROM ";
    char query[120];
    sprintf(query, "%s%s where %s =%d;", head, _STR_MTB, field_name, key);
    printf("%s\n", query);
    mysql_real_query(mysql, query, strlen(query));
}

void mysqldb_logout(MYSQL *mysql, int fd)
{
    mysqldb_delete(mysql, "(fd)", fd);
}

/* 更新数据 */
void mysqldb_update_addr4(MYSQL *mysql, char *name, long long *addr4)
{
    char *head = "UPDATE ";
    char query[100];
    printf("%lld", *addr4);
    sprintf(query, "%s%s SET addr4=\'%lld\' WHERE name = \'%s\';", head, _STR_MTB, *addr4, name);
    printf("%s\n", query);
    mysql_real_query(mysql, query, strlen(query));
}

/* 查询数据 */
void mysqldb_query(MYSQL *mysql, char *select_, char *where_, char *where__)
{
    char query[100] = {0};
    sprintf(query, "SELECT %s FROM %s WHERE %s = \"%s\";", select_, _STR_MTB, where_, where__);
    printf("%s", query);
    int t = mysql_real_query(mysql, query, strlen(query));
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row;
    if (row = mysql_fetch_row(res))
    {
        char tmp[100];
        sprintf(tmp, "%s", row[0]);
        if (tmp[0] != '(')
        {
            printf("??***??\n\n\n");
            sprintf(mysqldb_query_result, "%s", row[0]);
        }
        else
        {
            printf("??*************??\n\n\n");
            mysqldb_query_result[0] = '\0';
        }
    }
    mysql_free_result(res);
}

void mysqldb_query_who(MYSQL *mysql, int fd)
{
    char t[5];
    sprintf(t, "%d", fd);
    mysqldb_query(mysql, "name", "fd", t);
}

void mysqldb_query_addr4(MYSQL *mysql, char *name)
{
    mysqldb_query(mysql, "addr4", "name", name);
}

int mysqldb_exist(MYSQL *mysql, char *name)
{
    int return_value = 0;
    char query[100] = {0};
    char *head = "SELECT name FROM ";
    char *where = " WHERE name = ";
    char *left_quot = "\"";
    char *right_quot = "\"";
    sprintf(query, "%s%s%s%s%s%s%s", head, _STR_MTB, where, left_quot, name, right_quot, ";");
    int t = mysql_real_query(mysql, query, strlen(query));
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row;
    unsigned rlen = mysql_num_fields(res);
    if (row = mysql_fetch_row(res))
        return_value = 1;
    mysql_free_result(res);
    return return_value;
}
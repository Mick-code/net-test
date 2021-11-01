#include <mysql/mysql.h>
#include <stdio.h>
#include <unistd.h>
#include <unp.h>

#define _STR_MHOST "localhost"
#define _STR_MUSER "root"
#define _STR_MPASSWD ""
#define _STR_MDB "team02"
#define _STR_MTB "login"

/*
 * name, ip6 
*/

/* 错误检查 */
static inline void _mysql_check(MYSQL *con);

/* 连接mysql */
void mysqldb_connect(MYSQL *mysql);

/* 插入数据 */
void mysqldb_insert(MYSQL *mysql, char *field_name, char *message);

/* 登录 */
void mysqldb_login(MYSQL *mysql, int fd, char *name);

/* 删除数据 */
void mysqldb_delete(MYSQL *mysql, char *field_name, int key);

/* 登出 */
void mysqldb_logout(MYSQL *mysql, int fd);

/* 更新数据 */
void mysqldb_update_addr4(MYSQL *mysql, char *name, long long *addr4);

/* 查询数据 */
char mysqldb_query_result[100];
void mysqldb_query(MYSQL *mysql, char *select_, char *where_, char *where__);
void mysqldb_query_who(MYSQL *mysql, int fd);
void mysqldb_query_addr4(MYSQL *mysql, char *name);

/* 断开mysql连接 */
void close_connection(MYSQL *mysql);

/* 是否存在 */
int mysqldb_exist(MYSQL *mysql, char *name);
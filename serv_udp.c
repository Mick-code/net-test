// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unp.h>
#include "mysqldb.h"

#define PORT 33336

int serv_fd;
char buffer[MAXLINE];
char sen_buf[MAXLINE];

// Driver code
int main()
{
    /* MYSQL */
    MYSQL *database = mysql_init(NULL);
    mysqldb_connect(database);
    /* 套接口初始化 */
    struct sockaddr_in servaddr, clie_addr;
    serv_fd = Socket(AF_INET, SOCK_DGRAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&clie_addr, 0, sizeof(clie_addr));
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    Bind(serv_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    while (1)
    {
        int len = sizeof(clie_addr);
        int n = recvfrom(serv_fd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&clie_addr, &len);
        buffer[n] = '\0';
        if (n)
        {
            mysqldb_query_addr4(database, buffer);

            if (mysqldb_query_result)
            {
                if (mysqldb_query_result[0] == '\0')
                {
                    printf("shit ffffffff\n\n");
                    char *tmp;
                    tmp = inet_ntoa(clie_addr.sin_addr);
                    printf("%s %d\n", tmp, clie_addr.sin_port);
                    long long t = ((long long)(clie_addr.sin_addr.s_addr) << 32) + (long long)(clie_addr.sin_port);
                    mysqldb_update_addr4(database, buffer, &t);
                }
                else
                {
                    printf("shit232222222\n\n");
                    sendto(serv_fd, mysqldb_query_result, strlen(mysqldb_query_result), 0, (const struct sockaddr *)&clie_addr, sizeof(clie_addr));
                    printf("%s\n\n", mysqldb_query_result);
                }
            }
            else
            {
                printf("asdfjalksdjflkajd");
            }
        }
    }
    mysql_close(database);
    return 0;
}
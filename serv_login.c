#include "mysqldb.h"

#define BUF_LEN 1024   //设备读取缓存的大小
#define SERVPORT 33335 //服务器监听端口号

int main(void)
{
	// 创建数据连接对象, 需要和 mysql_close成对出现
	MYSQL *database = mysql_init(NULL);
	mysqldb_connect(database);
	//初始化
	int r = 0;
	char buf[BUF_LEN + 1];
	//网络服务器初始化
	int listen_fd, client_fd;	  //listen_fd:监听socket;client_fd:数据传输socket
	struct sockaddr_in serv_addr; //本地地址
	struct sockaddr_in clie_addr; //客户地址信息
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVPORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	//socket()
	listen_fd = Socket(AF_INET, SOCK_STREAM, 0);
	//bind()
	Bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
	Listen(listen_fd, SOMAXCONN);
	printf("start!\n");
	//select()
	fd_set readfds, testfds;
	FD_ZERO(&readfds);
	FD_SET(listen_fd, &readfds);
	int result;

	while (1)
	{
		testfds = readfds;
		result = Select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
		for (int fd = 0; fd < FD_SETSIZE; fd++)
		{
			if (FD_ISSET(fd, &testfds))
			{
				if (fd == listen_fd)
				{
					socklen_t sin_size = sizeof(struct sockaddr_in);
					client_fd = Accept(listen_fd, (struct sockaddr *)&clie_addr, &sin_size);
					int tmp = Recv(client_fd, buf, BUF_LEN, 0);
					char name[11];
					sprintf(name, buf);
					tmp = *(int *)buf = mysqldb_exist(database, name);
					Send(client_fd, buf, BUF_LEN, 0);
					if (tmp)
						close(client_fd);
					else
					{
						mysqldb_login(database, client_fd, name);
						mysqldb_query_who(database, client_fd);
						printf("\n%s\n", mysqldb_query_result);
						FD_SET(client_fd, &readfds);
					}
				}
				else
				{
					r = recv(fd, buf, BUF_LEN, 0);
					if (r < 0)
					{
						mysqldb_logout(database, fd);
						FD_CLR(fd, &readfds);
						close(fd);
						break;
					}
					else if (r == 0)
					{
						mysqldb_logout(database, fd);
						FD_CLR(fd, &readfds);
						close(fd);
						break;
					}
					else
					{
						//消息群发
						char message[BUF_LEN];
						mysqldb_query_who(database, fd);
						sprintf(message, "%s: %s",mysqldb_query_result, buf);
						printf("%s\n", message);
						for (int fd = 0; fd < FD_SETSIZE; fd++)
						{
							if ((FD_ISSET(fd, &readfds) && (fd != listen_fd)))
							{
								Send(fd, message, BUF_LEN, 0);
							}
						}
					}
				}
			}
		}
	}
	close(listen_fd);
	printf("goodbye!\n");
	mysql_close(database);
	return 0;
}

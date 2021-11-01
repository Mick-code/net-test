#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#include "client.h"
#include <windows.h>

using namespace std;

#define BACKLOG 10 //最大同时连接请求数
#define BUF_LEN 1024 //设备读取缓存的大小
#define SERVPORT 33335 //服务器端口A
#define FD_SETSIZE 1024

const char SITE[128] = "www.logblog.top\0";

#define USERNAME_LEN 1+10

SOCKET serv_fd;
char buf[BUF_LEN + 1];
SOCKET udp_serv_fd;
char udp_buf[BUF_LEN + 1];
SOCKADDR_IN udp_serv_addr;
SOCKADDR_IN serv_addr;
SOCKADDR_IN udp_remote_addr;
SOCKADDR_IN udp_local_addr;
char username[USERNAME_LEN]{};
char* send_ = NULL;
#include "main.h"

int main(int argc, char* argv[]) {
	//while (1) start();
	return 0;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	std::cout << "Hellow C++" << std::endl;

	//弹出一个消息框函数，第一个参数为副窗口一般写NULL，第二个参数是消息的内容，第三个参数是标题，第四个参数是按钮（常量）
	MessageBox(NULL, TEXT("Hellow C++"), TEXT("标题"), MB_OK);

	system("pause");
	return 0;
}
void start() {
	printf("login as:");
	scanf_s("%s", username, USERNAME_LEN);
	//初始化 DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//
	BOOL bKeepAlive = TRUE;
	int nRet = setsockopt(serv_fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(bKeepAlive));
	tcp_keepalive alive_in;
	tcp_keepalive alive_out;
	alive_in.keepalivetime = 30000;  // 30s  
	alive_in.keepaliveinterval = 30000; //30s  
	alive_in.onoff = TRUE;
	unsigned long ulBytesReturn = 0;
	nRet = WSAIoctl(serv_fd, SIO_KEEPALIVE_VALS, &alive_in, sizeof(alive_in), &alive_out, sizeof(alive_out), &ulBytesReturn, NULL, NULL);
	//获取服务器的IP
	printf("%s\n", SITE);
	struct hostent* host;
	if ((host = gethostbyname(SITE)) == NULL) {
		printf("Gethostbyname error!\n");
		exit(1);
	}
	printf("%s\n", inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
	//创建套接口
	serv_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	udp_serv_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	udp_serv_addr.sin_family = AF_INET;
	udp_serv_addr.sin_port = htons(33336);
	udp_serv_addr.sin_addr = *((struct in_addr*)host->h_addr);
	//udp_local_addr.sin_family = AF_INET;
	//udp_local_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	//udp_local_addr.sin_port = htons(60201);
	//bind(udp_serv_fd, (struct sockaddr*)&udp_local_addr, sizeof(udp_local_addr));
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVPORT);
	serv_addr.sin_addr = *((struct in_addr*)host->h_addr);
	if (connect(serv_fd, (struct sockaddr*)(&serv_addr),
		sizeof(struct sockaddr)) == -1) {
		printf("connect error!\n");
		closesocket(serv_fd);
		exit(1);
	}
	//memset(buf, 0, BUF_LEN);
	sprintf(buf, username);
	send(serv_fd, buf, BUF_LEN, 0);
	if (recv(serv_fd, buf, BUF_LEN, 0) > 0) { 
		if (*(int*)buf) { printf("用户名已被使用！\n"); return; }
		communicate();
	}
	closesocket(serv_fd);
	exit(1);
}

void communicate() {
	printf("Welcome!\n");

	std::thread t_read_keyboard(read_keyboard);

	std::thread t_udp_keep(udp_keep);

	fd_set readfds, testfds;
	FD_ZERO(&readfds);
	FD_SET(serv_fd, &readfds);
	FD_SET(udp_serv_fd, &readfds);
	int select_return;
	while (1)
	{
		testfds = readfds;
		select_return = select(FD_SETSIZE, &testfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0);
		for (int fd = 0; fd < FD_SETSIZE; fd++)
		{
			if (FD_ISSET(fd, &testfds))
			{
				if (fd == serv_fd)
				{
					int len = recv(fd, buf, BUF_LEN, 0);
					printf("[全体]%s\n", buf);
				}
				else if (fd == udp_serv_fd){
					int len = sizeof(udp_remote_addr);
					int n = recvfrom(udp_serv_fd, (char*)buf, BUF_LEN, 0, (struct sockaddr*)&udp_remote_addr, &len);
					buf[n] = '\0';
					if (buf[0] == '[') {
						printf("%s\n", buf);
					}
					else {
						long long addr4_i = 0;
						sscanf(buf,"%lld",&addr4_i);
						printf("%lld\n", addr4_i);
						udp_remote_addr.sin_addr.S_un.S_addr = addr4_i >> 32;
						udp_remote_addr.sin_port = addr4_i;
						sprintf(udp_buf, "%s%s: ", "[私信]", username);
						int len = strlen(udp_buf);
						while (*(send_) != '\0') udp_buf[len++] = *(send_++);
						udp_buf[len] = '\0';
						printf("***%s***", udp_buf);
						sendto(udp_serv_fd, udp_buf, strlen(udp_buf), 0, (const struct sockaddr*)&udp_remote_addr, sizeof(udp_remote_addr));
					}
				}
			}
		}
	}
}

void read_keyboard(void) {
	char buf[BUF_LEN];
	while (1)
	{
		char input[BUF_LEN];
		char to_[10] = { '\0' };
		gets_s(input, BUF_LEN);
		printf("%s\n", input);
		if (input[0] == 't' && input[1] == 'o' && input[2] == ' ') {
			int tmp = sscanf(input + 3, "%s", to_);
			send_ = input + 3 + strlen(to_) + 1;
			sendto(udp_serv_fd, to_, strlen(to_), 0, (const struct sockaddr*)&udp_serv_addr, sizeof(udp_serv_addr));
			printf("%s\n\n", to_);
		}
		else {
			send(serv_fd, input, BUF_LEN, 0);
			Sleep(3000);
		}
	}
}

void udp_keep() {
	sendto(udp_serv_fd, username, strlen(username), 0, (const struct sockaddr*)&udp_serv_addr, sizeof(udp_serv_addr));
	while (1)
	{
		Sleep(20000);
		sendto(udp_serv_fd, NULL, 0, 0, (const struct sockaddr*)&udp_serv_addr, sizeof(udp_serv_addr));
	}
}
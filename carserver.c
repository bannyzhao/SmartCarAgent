#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")
#define BUFSIZE 512
#define MOTOR_GO_STOP		printf("stop\n");
#define MOTOR_GO_FORWARD	printf("forward\n");
#define MOTOR_GO_BACK		printf("back\n");
#define MOTOR_GO_RIGHT		printf("right\n");
#define MOTOR_GO_LEFT		printf("left\n");


int main(int argc, char* argv[])
{
	
	char buf[BUFSIZE] = { 0xff,0x00,0x00,0x00,0xff };


	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//创建套接字
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//绑定IP和端口
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}

	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//循环接收数据
	SOCKET sClient;
	struct sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];

	int z = -1;

	while (1)
	{
		ZeroMemory(buf, BUFSIZE + 1);

		printf("Waiting connect...\n");
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !\r\n");
			continue;
		}
		printf("Received a connection：%s \r\n", inet_ntoa(remoteAddr.sin_addr));

		//接收数据
		//int ret = recv(sClient, revData, 255, 0);
		if ((z = recv(sClient, buf, sizeof buf,0)) > 0)
		{
			buf[z] = '\0';
			char *result = buf;
			if (strcmp(result, "forward") == 0 ) 
			{
				MOTOR_GO_FORWARD;
			}
			else if (strcmp(result, "back") == 0) 
			{
				MOTOR_GO_BACK;
			}
			else if (strcmp(result, "left") == 0)
			{
				MOTOR_GO_LEFT;
			}
			else if (strcmp(result, "right") == 0)
			{
				MOTOR_GO_RIGHT;
			}
			else if (strcmp(result, "stop") == 0)
			{
				MOTOR_GO_STOP;
			}
			
		}
	}

	closesocket(slisten);
	getchar();
	WSACleanup();
	return 0;
}
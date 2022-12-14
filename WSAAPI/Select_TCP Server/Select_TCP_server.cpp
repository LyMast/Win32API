#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment (lib, "ws2_32")

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

#define SERVERPORT		9000
#define BUFSIZE			512

struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
};

int nTotalSockets = 0;
SOCKETINFO* SocketInfoArray[FD_SETSIZE];			// 64

// 소켓 관리 함수
BOOL AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

// 오류 출력 함수
//void err_quit(const char *msg);
//void err_display(const char *mag);

int main(int argc, char** argv)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socker()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		printf("socket()");
		return -1;
	}

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);

	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		printf("bind()");
		return -1;
	}


	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		printf("listen()");
		return -1;
	}

	// 넌블로킹 소켓으로 전환
	u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if (retval == SOCKET_ERROR)
	{
		printf("ioctlsocket()");
		return -1;
	}

	// 데이터 통신에 사용할 변수
	FD_SET rset, wset;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, i = 1;

	while (1)
	{
		// 소켓 셋 초기화
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listen_sock, &rset);
		for (i = 0; i < nTotalSockets; i++)
		{
			if (SocketInfoArray[i]->recvbytes > SocketInfoArray[i]->sendbytes)
				FD_SET(SocketInfoArray[i]->sock, &wset);
			else
				FD_SET(SocketInfoArray[i]->sock, &rset);
		}

		// select()
		retval = select(0, &rset, &wset, NULL, NULL);
		if (retval == SOCKET_ERROR)
		{
			printf("select()");
			return -1;
		}

		// 소켓 셋 검사(1) : Client 접속 수용
		if (FD_ISSET(listen_sock, &rset)) 
		{
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET)
			{ 
				printf("accept()");
			}
			else
			{
				printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\n",
					inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
				// 소켓 정보 추가
				AddSocketInfo(client_sock);
			}
		}
		// 소켓 셋 검사 (2) : 데이터 통신
		for (i = 0; i < nTotalSockets; i++)
		{
			SOCKETINFO* ptr = SocketInfoArray[i];
			if (FD_ISSET(ptr->sock, &rset))
			{
				// 데이터 받기
				retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
				if (retval == SOCKET_ERROR)
				{
					printf("recv()");
					RemoveSocketInfo(i);
					continue;
				}
				else if (retval == 0)
				{
					RemoveSocketInfo(i);
					continue;
				}
				ptr->recvbytes = retval;
				// 받은 데이터 출력
				addrlen = sizeof(clientaddr);
				getpeername(ptr->sock, (SOCKADDR*)&clientaddr, &addrlen);
				ptr->buf[retval] = '\0';
				printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), ptr->buf);
			}
			if (FD_ISSET(ptr->sock, &wset))
			{
				// 데이터 보내기
				retval = send(ptr->sock, ptr->buf + ptr->sendbytes,
					ptr->recvbytes - ptr->sendbytes, 0);
				if (retval == SOCKET_ERROR)
				{
					printf("send()");
					RemoveSocketInfo(i);
					continue;
				}
				ptr->sendbytes += retval;
				if (ptr->recvbytes == ptr->sendbytes)
				{
					ptr->recvbytes = ptr->sendbytes = 0;
				}
			}
		}
	}

	
	// 윈속 종료
	WSACleanup();
	return 0;
}



// 소켓 정보 추가

BOOL AddSocketInfo(SOCKET sock)
{
	if (nTotalSockets >= FD_SETSIZE)
	{
		printf("[오류] 소켓 정보를 추가할 수 없습니다.!\n");
		return FALSE;
	}
	SOCKETINFO* ptr = new SOCKETINFO;
	if (ptr == NULL)
	{
		printf("[오류] 메모리가 부족합니다.");
		return FALSE;
	}

	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	SocketInfoArray[nTotalSockets++] = ptr;

	return TRUE;

}
	

// 소켓 정보 삭제
void RemoveSocketInfo(int nIndex)
{
	SOCKETINFO* ptr = SocketInfoArray[nIndex];

	// 클라이언트 정보 얻기
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP 서버] 클라이언트 종료 : IP 주소 = %s,  포트 번호 = %d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	closesocket(ptr->sock);
	delete ptr;

	if (nIndex != (nTotalSockets - 1))
	{
		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets - 1];
	}

	--nTotalSockets;
}



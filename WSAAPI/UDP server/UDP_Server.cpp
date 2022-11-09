#pragma comment (lib, "ws2_32")

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>


#define SERVERPORT	9000
#define BUFSIZE		512


int main(int argc, WCHAR* argv[])
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;


	// socket()
	SOCKET sock = socket(AF_INET6, SOCK_DGRAM, 0); // UDP
	if (sock == INVALID_SOCKET)
		return -1;

	// Initialize Socket Struct
	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(SERVERPORT);

	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		return -1;

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN6 clientaddr;
	char buf[BUFSIZE + 1];
	int addrlen;

	while (1)
	{
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(SOCKADDR*)&clientaddr, &addrlen);
		if (retval == SOCKET_ERROR)
		{
			continue;
		}

		// 주소 변환 (IPv6 -> 문자열)
		WCHAR ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
			continue;
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr), NULL, ipaddr, &ipaddrlen);

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[UDP/ %s] %s\n", ipaddr, buf);

		// 데이터 보내기
		sendto(sock, buf, retval, 0, (SOCKADDR*)&clientaddr, sizeof(clientaddr));


	}

	closesocket(sock);

	WSACleanup();
	return 0;


}



#pragma comment (lib, "ws2_32")

#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>

#define SERVERIP	"::1"
#define SERVERPORT	9000
#define BUFSIZE		512

int main(int argc, char* argv[])
{
	int retval;
	
	// Initialize Winsock
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket();
	SOCKET sock = socket(AF_INET6, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		return -1;

	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET;
	int addrlen = sizeof(serveraddr);
	WSAStringToAddress( (LPWSTR)SERVERIP, AF_INET6, NULL, (SOCKADDR*)&serveraddr, &addrlen);
	serveraddr.sin6_port = htons(SERVERPORT);


	//
	SOCKADDR_IN6 peeraddr;
	int len;
	char buf[BUFSIZE + 1];

	
	while (1)
	{
		printf("\n[���� ������] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

		printf("[UDP Ŭ���̾�Ʈ] %d ����Ʈ�� ���½��ϴ�. \n", retval);


		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR *)&peeraddr, &addrlen);


		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr)))
		{
			printf("Wrong Data");
			continue;
		}

		buf[retval] = '\0';
		printf("[UDP Ŭ���̾�Ʈ] %����Ʈ�� �޾ҽ��ϴ�. \n", retval);
		printf("[���� ������ ] %s\n", buf);

	}
	closesocket(sock);

	WSACleanup();
	return 0;

}
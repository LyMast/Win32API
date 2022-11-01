#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT	9000
#define BUFSIZE		512

// Fuction to Print Socket Error Message and Quir Process

// Function to Display Socket Function Error

//int _wmain(int argc, WCHAR* argv[])
int main(int argc, char* arg[])
{
	int retval;

	// Initailize Socket
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// Socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		return -1;

	// Bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		return -1;

	// Listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		return -1;

	// Variable to Use in Data Communication
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (true)
	{
		// Accept()addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			break;
		}
		

		// Print Connected Client Information
		printf("\n [TCP server] Client connet : IP address = %s, Port NUM = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Data Communication with Client
		while (true)
		{
			// Receive Data
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				break;
			}
			else if (retval == 0)
				break;


			// Print Revieved Data
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), 
				ntohs(clientaddr.sin_port), buf);


			// Send Data
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR)
			{
				break;
			}
		}

		// Closesocket()
		closesocket(client_sock);
		printf("[TCP server] terminate Client : IP address = %s, Port NUM = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	}

	// CloseSocket()
	closesocket(listen_sock);

	// Terminate Winsock
	WSACleanup();
	return 0;
}
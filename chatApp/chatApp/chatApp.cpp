// chatApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
	/////////////////////// SERVER /////////////////////////////
	// Initialize Winsock
	WSADATA wsData;

	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (iResult != 0) {
		cerr << "Initialize winsock failed! - Exit" << endl;
		return 1;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Creating socket failed! - Exit" << endl;
		return 1;
	}

	// Bind the socket to an IP address and port 
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Wait for a connection 
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST]; // client's remote name
	char service[NI_MAXSERV]; // Service the client is connect on
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << "connect on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}

	// Close listening socket 
	closesocket(listening);

	// While loop: accept and echo message back to client 
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		//Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv() - Exit" << endl;
			break;
		}
		if (bytesReceived == 0)
		{
			cout << "Client Disconnected " << endl;
			break;
		}

		//Echo message back to client:
		send(clientSocket, buf, bytesReceived + 1, 0);
	}


	// Close the socket
	closesocket(clientSocket);

	// Shutdown winsock
	WSACleanup();
	/////////////////////////////////////////////////////////

	cin.get();
    return 0;
}


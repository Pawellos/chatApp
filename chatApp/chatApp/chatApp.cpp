// chatApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <sstream>
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

	fd_set master;
	FD_ZERO(&master);
	FD_SET(listening, &master);

	while (true)
	{
		fd_set copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);
				// Add new connection to the list of connected clients
				FD_SET(client, &master);
				// Send a welcone message to the connected client
				string welconeMsg = "Welcome on chat server! \r\n";
				send(client, welconeMsg.c_str(), welconeMsg.size() + 1, 0);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				//Receive message 
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					//drop the client 
					closesocket(sock);
					FD_CLR(sock, &master);

				}
				else
				{
					// Send message to other clients, and definiaetly NOT listening socket
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buf << "\r";
							string strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}

			}

		}
	}

	//// Wait for a connection 
	//sockaddr_in client;
	//int clientSize = sizeof(client);

	//SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	//char host[NI_MAXHOST]; // client's remote name
	//char service[NI_MAXSERV]; // Service the client is connect on
	//ZeroMemory(host, NI_MAXHOST);
	//ZeroMemory(service, NI_MAXSERV);

	//if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	//{
	//	cout << host << "connect on port " << service << endl;
	//}
	//else
	//{
	//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
	//	cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	//}

	//// Close listening socket 
	//closesocket(listening);

	//// While loop: accept and echo message back to client 
	//char buf[4096];

	//while (true)
	//{
	//	ZeroMemory(buf, 4096);

	//	//Wait for client to send data
	//	int bytesReceived = recv(clientSocket, buf, 4096, 0);
	//	if (bytesReceived == SOCKET_ERROR)
	//	{
	//		cerr << "Error in recv() - Exit" << endl;
	//		break;
	//	}
	//	if (bytesReceived == 0)
	//	{
	//		cout << "Client Disconnected " << endl;
	//		break;
	//	}
	//	cout << string(buf, 0, bytesReceived) << endl;

	//	//Echo message back to client:
	//	send(clientSocket, buf, bytesReceived + 1, 0);
	//}


	// Close the socket
	//closesocket(clientSocket);

	// Shutdown winsock
	WSACleanup();
	/////////////////////////////////////////////////////////

	cin.get();
	return 0;
}


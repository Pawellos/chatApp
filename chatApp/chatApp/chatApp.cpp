// chatApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
	/////////////////////// CLIENT /////////////////////////////
	string ipAddress = "127.0.0.1";
	int port = 54000;
	// Initialize Winsock
	WSAData data;
	int wsResult = WSAStartup(MAKEWORD(2, 2), &data);
	if (wsResult != 0)
	{
		cerr << "Start winsock failed - Exit" << endl;
		return 1;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Creating socket failed! - #" << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// Fill in a hint structure 
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server 
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Connect to server failed! - Error #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// Do-while loop to send and receive data 
	char buf[4096];
	string userInput;

	do
	{
		// Promt the user for some text
		cout << "> ";
		getline(cin, userInput);

		if (userInput.size() > 0)
		{
			// Send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					// Echo response to console
					cout << "SERVER " << string(buf, 0, bytesReceived) << endl;
				}
			}
		}
	} while (userInput.size() > 0);

	// Close down everything
	closesocket(sock);
	WSACleanup();

	/////////////////////////////////////////////////////////
	cin.get();
    return 0;
}


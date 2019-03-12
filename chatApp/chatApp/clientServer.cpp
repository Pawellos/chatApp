#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include "clientServer.h"
#pragma comment(lib, "Ws2_32.lib")

TCPserver::TCPserver(std::string ipAddress, int port, MessageReceivedHandler handler)
	: m_ipAddress(ipAddress), m_port(port), MessageReceived(handler)
{
}

TCPserver::TCPserver()
{

}

//TCPserver::~TCPserver()
//{
//	WSACleanup(); 
//}

bool TCPserver::init()
{
	WSADATA wsData;

	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsData);

	return iResult == 0;
}

SOCKET TCPserver::createSocket()
{
	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); 
	if (listening != INVALID_SOCKET)
	{ 
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);
		// Bind the socket to an IP address and port 
		int bindOk = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOk != SOCKET_ERROR)
		{
			int listenOk = listen(listening, SOMAXCONN);
			if(listenOk == SOCKET_ERROR)
			{
				return -1;
			}
		}
		else 
		{
			return -1;
		}
		return listening;
	}
}

SOCKET TCPserver::waitForConnection(SOCKET listening)
{
	SOCKET client = accept(listening, NULL, NULL);

	return client;
}

void TCPserver::run()
{
	char buf[MAX_BUFFER_SIZE];

	while (true)
	{
		//Create a listening socket
		SOCKET listening = createSocket();
		if (listening == INVALID_SOCKET)
		{
			break;
		}
		//Wait for connection
		SOCKET client = waitForConnection(listening);
		if (client != INVALID_SOCKET)
		{
			closesocket(listening);
			int bytesReceived = 0;
			do
			{
				ZeroMemory(buf, MAX_BUFFER_SIZE);
				bytesReceived = recv(client, buf, MAX_BUFFER_SIZE, 0);
				if (bytesReceived > 0)
				{
					if (MessageReceived != NULL)
					{
						MessageReceived(this, client, std::string(buf, 0, bytesReceived));
					}
				}
			} while (bytesReceived > 0);
			closesocket(client);
		}
		
		//Loop receive/ send

	}
}

void TCPserver::sendMsg(int clientSocket, std::string msg)
{
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

void TCPserver::cleanup()
{
	WSACleanup();
}

bool TCPclient::init()
{
	WSADATA wsData;

	int wsResult;
	wsResult = WSAStartup(MAKEWORD(2, 2), &wsData);

	return wsResult == 0;
}

SOCKET TCPclient::createSocket()
{
	// Create a socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock != INVALID_SOCKET)
	{
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);
		return sock;
	}
	else
	{
		WSACleanup();
		return -1;
	}
}

void TCPclient::connectClientServer()
{
	//int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	//if (connResult != SOCKET_ERROR)
	//{
	//	
	//	return connResult;
	//}
	//else
	//{
	//	closesocket(sock);
	//	WSACleanup();
	//	return -1;
	//}
}

void TCPclient::close()
{
	//closesocket(sock);
	//WSACleanup();
}

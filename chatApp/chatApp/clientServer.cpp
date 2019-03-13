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
			if (listenOk == SOCKET_ERROR)
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
			//Loop receive/ send
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
	}
}

void TCPserver::run_multiple()
{
	char buf[MAX_BUFFER_SIZE];

	//Create a listening socket
	SOCKET listening = createSocket();
	if (listening == INVALID_SOCKET)
	{
		cleanup();
	}
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
				std::string welconeMsg = "Welcome on chat server! \r\n";
				send(client, welconeMsg.c_str(), welconeMsg.size() + 1, 0);
			}
			else
			{
				//char buf[4096];
				ZeroMemory(buf, MAX_BUFFER_SIZE);

				//Receive message 
				int bytesIn = recv(sock, buf, MAX_BUFFER_SIZE, 0);
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
							std::ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buf << "\r";
							std::string strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
			}
		}
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

TCPclient::TCPclient(std::string ipAddress, int port)
	: m_ipAddress(ipAddress), m_port(port)
{
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
		std::cerr << "Creating socket failed! - #" << WSAGetLastError() << std::endl;
		closeSock(sock);
		return -1;
	}
}

int TCPclient::connectClientServer(SOCKET sock)
{
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_port);
	inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Connect to server failed! - Error #" << WSAGetLastError() << std::endl;
		closeSock(sock);
		return -1;
	}
	else
		return connResult;
}

void TCPclient::run()
{
	char buf[MAX_BUFFER_SIZE];

	//Create a listening socket
	SOCKET sock = createSocket();
		if (sock == INVALID_SOCKET)
		{
			closeSock(sock);
		}
		//Connect to server
		connectClientServer(sock);
	// Do-while loop to send and receive data 
	std::string userInput;

	do
	{
		// Promt the user for some text
		std::cout << "> ";
		getline(std::cin, userInput);
		//std::cin >> userInput;

		if (userInput.size() > 0)
		{
			// Send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				ZeroMemory(buf, MAX_BUFFER_SIZE);
				int bytesReceived = recv(sock, buf, MAX_BUFFER_SIZE, 0);
				if (bytesReceived > 0)
				{
					// Echo response to console
					std::cout << "SERVER " << std::string(buf, 0, bytesReceived) << std::endl;
				}
			}
		}
	} while (userInput.size() > 0);
	closeSock(sock);
}

void TCPclient::closeSock(SOCKET sock)
{
	closesocket(sock); 
	WSACleanup();
}

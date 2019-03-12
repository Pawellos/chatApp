#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <sstream>
#pragma comment(lib, "Ws2_32.lib")

constexpr auto MAX_BUFFER_SIZE = 49152;

class TCPserver;
typedef void(*MessageReceivedHandler)(TCPserver* server, int socketId, std::string msg);

class TCPserver
{
private:
	int m_port;
	std::string m_ipAddress;
	MessageReceivedHandler MessageReceived; 
public:
	//constructor default
	TCPserver();
	//constructor
	TCPserver(std::string ipAddress, int port, MessageReceivedHandler handler);
	//Initialize winsock
	bool init();
	//Create a winsock 
	SOCKET createSocket();
	//Wait for connection
	SOCKET waitForConnection(SOCKET listening);
	//Receive loop
	//The main processing loop
	void run();
	//Send back message
	void sendMsg(int clientSocket, std::string msg);
	//Clean up
	void cleanup();

}; 


class TCPclient
{
private:
	int m_port;
	std::string m_ipAddress;

public:
	//Initialize winsock
	bool init();
	//Create a winsock 
	SOCKET createSocket();
	// Connect to server 
	void connectClientServer();
	//close down everything
	void close();
};
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
	//Bind socket with IP/port 
	SOCKET bindSocket(SOCKET listening);
	//Wait for connection
	SOCKET waitForConnection(SOCKET listening);
	//The main processing loop for echo server
	void run();
	//The main processing loop for multiple clients server
	void run_multiple();
	//Send back message
	void sendMsg(int clientSocket, std::string msg);
	//Receive data
	void recvData(SOCKET client);
	//Clean up 
	void cleanup();

};


class TCPclient
{
	friend class TCPpresenter;
private:
	int m_port;
	std::string m_ipAddress;
public:
	//constructor
	TCPclient(std::string ipAddress, int port);
	//Initialize winsock
	bool init();
	//Create a winsock 
	SOCKET createSocket();
	// Connect to server 
	int connectClientServer(SOCKET sock);
	//Run send and recv message
	void run();
	//Send and receive data in loop
	void send_recv_data(SOCKET sock);
	//Receive data
	void recvData(SOCKET client);
	//close down everything
	void closeSock(SOCKET sock);
};

class TCPpresnter
{
	friend class TCPclient;
private:



public:
	static void echoResponse(char * buffer, int sizeRecv);
	static void promtUser(std::string user);

};
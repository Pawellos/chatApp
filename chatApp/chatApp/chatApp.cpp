// chatApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <sstream>
#include "clientServer.h"
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

void Listener_MessageReceived(TCPserver* listener, int client, string msg);

void runServer(TCPserver s)
{
	cout << "serwer start! \r\n";
	s.run_multiple();
};

void runClient(TCPclient s)
{
	cout << "client start \r\n";
	s.run();
};

int main()
{
	//	thread secondClient; 
	/////////////////////// SERVER //////////////////////////////////
	TCPserver server("127.0.0.1", 54000, Listener_MessageReceived);


	if (server.init())
	{
		server.run_multiple();
		//thread mainServer(runServer, server);
		//mainServer.join();
	}

	///////////////////////// CLIENT //////////////////////////////////
	TCPclient client("127.0.0.1", 54000);

	//if (client.init())
	//{
	//	client.run();
	////	thread firstClient(runClient, client);
	////	firstClient.join();
	//}
	////////////////////////////////////////////////////////////////
	//server.init();
	//thread mainServer(runServer, server);
	//client.init();
	//thread firstClient(runClient, client);
	//client.init();
	//thread secondClient(runClient, client);
	//mainServer.join();
	//firstClient.join();
	//secondClient.join();
	cin.get();
	return 0;
}

void Listener_MessageReceived(TCPserver * listener, int client, string msg)
{
	listener->sendMsg(client, msg);
}
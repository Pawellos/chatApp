// chatApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include "clientServer.h"
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

void Listener_MessageReceived(TCPserver* listener, int client, string msg);

int main()
{
	/////////////////////// SERVER //////////////////////////////////
	//TCPserver server("127.0.0.1", 54000, Listener_MessageReceived);

	//if (server.init())
	//{ 
	//	server.run_multiple();
	//}

	TCPclient client("127.0.0.1", 54000, Listener_MessageReceived);
	if (client.init())
	{
		client.run();
	}


	////////////////////////////////////////////////////////////////

	cin.get();
	return 0;
}

void Listener_MessageReceived(TCPserver * listener, int client, string msg)
{
	listener->sendMsg(client, msg);
}

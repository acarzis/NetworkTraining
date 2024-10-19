#pragma once 

#include <winsock2.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

using std::string;
using namespace std;

class Server
{
private:
	WSADATA				wsaData = { 0 };
	SOCKET				mListeningSocket = { 0 };
	SOCKET				clientSocket = { 0 };
	SOCKADDR_IN			ServerAddr = { 0 };
	SOCKADDR_IN			ClientAddr = { 0 };
	bool				mNonBlockMode;
	static vector<int>	mClientList;

	static char*		m_IncomingBuffer;
	static int			m_IncomingBufferSize;
	static std::mutex	m_IncomingBufferLock;
	static std::mutex	mClientListLock;

private:
	void static AddToIncomingBuffer(char* data);

public:
	static void processClients();
	static void processMessages();
	static int ReadXBytes(int clientSocket, unsigned int x, char* buffer, bool& disconnect);
	SOCKET WaitForConnection();
	Server(int listeningPort, bool nonblock = false);
	~Server();
	void AddToClientList(int clientSocket);
	static void RemoveFromClientList(int clientSocket);
	void startThreads();

	std::thread processClientsThread;
	std::thread processMessagesThread;
};



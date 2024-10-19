#include <vector>
#include <thread>
#include <iostream> 

#include "Server.h"
#include "Header Files/DataFrame.h"
#include "Header Files/Utils.h"

using namespace std;

// static members
vector<int>	Server::mClientList;
char* Server::m_IncomingBuffer = nullptr;
int Server::m_IncomingBufferSize = 0;
std::mutex Server::m_IncomingBufferLock;
std::mutex Server::mClientListLock;


Server::Server(int listeningPort, bool nonblock): mNonBlockMode(nonblock)
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		throw new std::exception("WSAStartup Failure");

	mListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (nonblock)
	{
		u_long mode = 1;  // 1 to enable non-blocking socket
		if (ioctlsocket(mListeningSocket, FIONBIO, &mode) != 0)
			throw new std::exception("set non-blocking mode exception");
	}

	if (mListeningSocket == INVALID_SOCKET)
		throw new std::exception("Socket Creation Failure");

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(listeningPort);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(mListeningSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)) < 0)
		throw new std::exception("bind failure");

	if (listen(mListeningSocket, 5) < 0)
		throw new std::exception("listen failure");

	cout << "Server created. Port " << listeningPort << endl;
}

Server::~Server()
{
	closesocket(clientSocket);
	closesocket(mListeningSocket);
	WSACleanup();
}

SOCKET Server::WaitForConnection()
{
	cout << "Listening for connection" << endl;

	int ClientAddrLen = sizeof(ClientAddr);
	if (!mNonBlockMode)
	{
		clientSocket = accept(mListeningSocket, (SOCKADDR*)&ClientAddr, &ClientAddrLen);

		if (clientSocket < 0)
			throw std::exception("accept failure");
	}
	else
	{
		while (1) {
			fd_set readfds;
			int max_sd, activity;

			// Clear the socket set
			FD_ZERO(&readfds);

			// Add server socket to the set
			FD_SET(mListeningSocket, &readfds);
			max_sd = mListeningSocket;

			// Wait for activity on any socket
			activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
			if (activity < 0) {
				throw std::exception("select error");
			}

			// If server socket has activity, it's a new connection
			if (FD_ISSET(mListeningSocket, &readfds)) {
				if ((clientSocket = accept(mListeningSocket, (struct sockaddr*)&ClientAddr, &ClientAddrLen)) < 0) {
					throw std::exception("accept failure");
				}
				break;
			}
		}
	}
	cout << "Connection established, socket fd is " << clientSocket << endl;
	return clientSocket;
}

//static
int Server::ReadXBytes(int clientSocket, unsigned int bytesToRead, char* buffer, bool &disconnect)
{
	int bytesRead = 0, result = 0;
	fd_set read_set;
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;	// TO DO: add as argument or constant

	FD_ZERO(&read_set);
	FD_SET(clientSocket, &read_set);

	int r = select(clientSocket + 1, &read_set, NULL, NULL, &timeout);

	if (r < 0) {
		throw std::exception("ReadXBytes select failure");
	}

	if (r == 0) {
		// Timeout - handle that. You could try waiting again, close the socket...
	}

	if (r > 0) {
		int result;
		while (bytesRead < bytesToRead)
		{
			result = recv(clientSocket, buffer + bytesRead, bytesToRead - bytesRead, 0);
			if (result == 0)
			{
				// graceful disconnect
				cout << "ReadXBytes, graceful disconnect, socket:" << clientSocket << endl;
				disconnect = true;
				break;
			}
			if (result < 1)
			{
				// disconnect
				cout << "ReadXBytes, disconnect, socket: " << clientSocket << endl;
				disconnect = true;
				break;
			}
			bytesRead += result;
		}
	}
	return bytesRead;
}


//static
void Server::processClients()
{
	char buf[100];

	cout << "processClients" << endl;

	while (1)
	{
		mClientListLock.lock();
		for (int x = 0; x < mClientList.size(); x++)
		{
			try {
				bool disconnect = false;
				int count = ReadXBytes(mClientList[x], 100, buf, disconnect);
				cout << "processClients, read #bytes: " << count << endl;

				if (count > 0)
				{
					char temp[101];
					memset(temp, 0, 101);
					memcpy(temp, buf, count);

					cout << "processClients, received data: " << temp << endl;

					AddToIncomingBuffer(temp);
					cout << "Client: " << mClientList[x] << " Received: " << temp << endl;
				}

				if (disconnect)
				{
					cout << "Client: " << mClientList[x] << " disconnected." << endl;
					closesocket(mClientList[x]);
					RemoveFromClientList(mClientList[x]);
					break;
				}
			}

			catch (exception ex)
			{
				cout << "processClients exception: " << ex.what() << endl;
			}
		}
		mClientListLock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Server::AddToClientList(int clientSocket)
{
	mClientListLock.lock();
	mClientList.push_back(clientSocket);
	mClientListLock.unlock();
}

void Server::RemoveFromClientList(int clientSocket)
{
	std::vector<int>::const_iterator t = std::find(mClientList.begin(), mClientList.end(), clientSocket);
	if (t != mClientList.end())
	{
		mClientList.erase(t);
	}
}


void Server::AddToIncomingBuffer(char* data)
{

	cout << "AddToIncomingBuffer length: " << strlen(data) << endl;

	m_IncomingBufferLock.lock();

	char *newbuffer = (char *) malloc((m_IncomingBufferSize + strlen(data) + 1) * sizeof(char));

	cout << "newbuffer: " << (void*) newbuffer << endl;

	memset(newbuffer, 0, m_IncomingBufferSize + strlen(data) + 1);
	memcpy(newbuffer, m_IncomingBuffer, m_IncomingBufferSize);
	memcpy(newbuffer + m_IncomingBufferSize, data, strlen(data));

	if (m_IncomingBuffer != nullptr)
	{
		cout << "AddToIncomingBuffer6 " << endl;	
		cout << "free(): " << (void*) m_IncomingBuffer << endl;
		free(m_IncomingBuffer);
		m_IncomingBuffer = nullptr;
	}

	m_IncomingBuffer = newbuffer;
	m_IncomingBufferSize += strlen(data);

	m_IncomingBufferLock.unlock();

	cout << "AddToIncomingBuffer m_IncomingBufferSize: " << m_IncomingBufferSize << endl;
	cout << "AddToIncomingBuffer buffer: " << m_IncomingBuffer << endl;
}


// static
void Server::processMessages()
{
	cout << "processMessages" << endl;

	while (1)
	{
		m_IncomingBufferLock.lock();

		if (Server::m_IncomingBufferSize > 0)
		{		
			cout << "processMessages m_IncomingBufferSize: " << Server::m_IncomingBufferSize << endl;

			std::vector<std::string> tokens = Utils::split(string(m_IncomingBuffer), DataFrame::TRAILER);  // note: trailer is removed

			if (tokens.size() > 1)
			{
				cout << "processMessages, #tokens: " << tokens.size() << ", tokens[0] size: " << tokens[0].size() << endl;

				// remove header; trailer is already removed
				string t = tokens[0];
				string t2 = t.substr(strlen(DataFrame::HEADER), t.size() - strlen(DataFrame::HEADER)); 

				cout << "processMessages, length: " <<  t2.size() << " t2: " << t2 << endl;

				DataFrame t3 = DataFrame::Unpack(t2);

				cout << "processMessages: " << t3.getUserId() << " " << t3.getxPosRandom() << " " << t3.getyPosRandom() << " " << t3.getSendString() << endl;

				m_IncomingBufferSize -= (t2.size() + strlen(DataFrame::HEADER) + strlen(DataFrame::TRAILER));
				char* newbuffer = (char*) malloc((m_IncomingBufferSize + 1) * sizeof(char));
				memset(newbuffer, 0, m_IncomingBufferSize + 1);

				if (m_IncomingBuffer != nullptr)
				{
					cout << "freeing m_IncomingBuffer: " << (void*)m_IncomingBuffer << endl;
					free(m_IncomingBuffer);
				}
				m_IncomingBuffer = newbuffer;
			}
		}
		m_IncomingBufferLock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	};
}

void Server::startThreads()
{
	processClientsThread = std::thread(processClients);
	processMessagesThread = std::thread(processMessages);
}


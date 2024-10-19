#include "ws2tcpip.h"
#include <string>
#include <exception>
#include "Client/Client.h"

using std::string;

Client::Client(string address, int port)
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		throw new std::exception("WSAStartup Failure");

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
		throw new std::exception("Socket Creation Failure");

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);

	if (inet_pton(AF_INET, address.c_str(), &ServerAddr.sin_addr) <= 0)
		throw new std::exception("Socket Creation Failure");
}

Client::~Client()
{
	closesocket(s);
	WSACleanup();
}

int Client::Connect()
{
	// assumes blocking connect call
	int result = connect(s, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
	if (result == SOCKET_ERROR)
		throw std::exception("Socket Connect Failure");

	return result;
}

int Client::SendData(const char *dataToSend, int dataLength)
{
	int numbytes = send(s, dataToSend, dataLength, 0);
	if (numbytes == SOCKET_ERROR)
		throw std::exception("SendData Failure");

	return numbytes;
}

int Client::Disconnect()
{
	shutdown(s, SD_BOTH);
	return closesocket(s);
}

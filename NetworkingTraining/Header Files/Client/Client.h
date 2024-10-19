#include <winsock2.h>
#include <string>

using std::string;

class Client
{
	WSADATA			wsaData;
	SOCKET			s = 0;
	SOCKADDR_IN		ServerAddr;


public:
	Client(string address, int port);
	~Client();
	int Connect();
	int Disconnect();
	int SendData(const char* dataToSend, int dataLength);
};


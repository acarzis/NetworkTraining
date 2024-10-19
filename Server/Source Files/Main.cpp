#include "Server.h"
#include <iostream>

using namespace std;


int main()
{
	char buf[100];
	Server s(5000, true);
	s.startThreads();

	while (1)
	{
		try {
			int clientSock = s.WaitForConnection();
			s.AddToClientList(clientSock);
		}
		catch (exception ex)
		{
			cout << "server main() exception: " << ex.what() << endl;
			break;
		}
	}
}


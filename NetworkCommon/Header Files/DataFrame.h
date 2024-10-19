#pragma once

#include <string>
using namespace std;

class DataFrame
{
private:
	long int m_userId = 0;
	int m_xPosRandom = 0;
	int m_yPosRandom = 0;
	char m_sendString[100] = {};

public:
	static const char HEADER[4];
	static const char TRAILER[4];

	DataFrame(int userid);
	void setUserId(int userid);
	void setxPosRandom(int xpos);
	void setyPosRandom(int ypos);
	void setSendString(const char *sendstring);
	int getUserId();
	int getxPosRandom();
	int getyPosRandom();
	string getSendString();

	const std::string Pack();
	const static DataFrame Unpack(const std::string data);
};


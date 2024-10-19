#include "pch.h"
#include <string>
#include <vector>

#include "base64.h"
#include "Utils.h"
#include "DataFrame.h"

// static members
const char DataFrame::HEADER[4] = "---";
const char DataFrame::TRAILER[4] = "...";


DataFrame::DataFrame(int userid) : m_userId(userid)
{
}

void DataFrame::setUserId(int userid)
{
	m_userId = userid;
}

void DataFrame::setxPosRandom(int xpos)
{
	m_xPosRandom = xpos;
}

void DataFrame::setyPosRandom(int ypos)
{
	m_yPosRandom = ypos;
}

void DataFrame::setSendString(const char* sendstring)
{
	strcpy_s(m_sendString, sendstring);
}

int DataFrame::getUserId()
{
	return m_userId;
}
int DataFrame::getxPosRandom()
{
	return m_xPosRandom;
}

int DataFrame::getyPosRandom()
{
	return m_yPosRandom;
}

string DataFrame::getSendString()
{
	return m_sendString;
}


// TO DO:  Transmitting base64 encoded string data is inneficient.
//	 	   Base64 encoding is just guaranteeing our header and trailer tokens won't be present in the data 	
//		   This mechanism should be improved	

const std::string DataFrame::Pack()
{
	std::string toPack = std::to_string(m_userId) + ",";
	toPack += std::to_string(m_xPosRandom) + ",";
	toPack += std::to_string(m_yPosRandom) + ",";
	toPack += m_sendString;
	toPack = base64_encode(toPack);

	std::string output = HEADER + toPack + TRAILER;
	return output;
}

const DataFrame DataFrame::Unpack(const std::string data)
{
	std::string toUnpack = base64_decode(data);
	std::vector<std::string> tokens = Utils::split(toUnpack, ",");

	DataFrame result(std::stoi(tokens[0]));	
	result.m_xPosRandom = std::stoi(tokens[1]);
	result.m_yPosRandom = std::stoi(tokens[2]);
	strcpy_s(result.m_sendString, tokens[3].c_str());
	return result;
}

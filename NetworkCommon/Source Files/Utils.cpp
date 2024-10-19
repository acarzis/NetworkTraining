#include "pch.h"

#include "Utils.h"

// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
std::vector<std::string> Utils::split(const std::string& s, const std::string& delimiter) 
{
    std::string copy(s);
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = copy.find(delimiter)) != std::string::npos) {
        token = copy.substr(0, pos);
        tokens.push_back(token);
        copy.erase(0, pos + delimiter.length());
    }
    tokens.push_back(copy);

    return tokens;
}

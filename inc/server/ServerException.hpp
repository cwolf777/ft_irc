#pragma once
#include <exception>
#include <string>

class ServerException : public std::exception
{
private:
    std::string _message;

public:
    ServerException(const std::string &msg) : _message(msg) {}
    const char *what() const noexcept override { return _message.c_str(); }
};

#include "Client.hpp"

Client::Client() : _fd(-1), _nickname(""), _registered(false), _is_op(false), _passwordCorrect(false) {}

Client::Client(int fd, std::string nickname, bool is_registerd, bool is_op) : _fd(fd), _nickname(nickname), _registered(is_registerd), _is_op(is_op) {}

Client::Client(int fd, std::string nickname, bool is_registerd, bool is_op, bool passwordCorrect) : _fd(fd), _nickname(nickname), _registered(is_registerd), _is_op(is_op), _passwordCorrect(passwordCorrect) {}

Client::Client(const Client &other) : _fd(other._fd), _nickname(other._nickname), _registered(other._registered), _is_op(other._is_op), _passwordCorrect(other._passwordCorrect) {}

Client &Client::operator=(const Client &other)
{
    if (this == &other)
        return *this;

    _nickname = other._nickname;
    _is_op = other._is_op;
    _fd = other._fd;
    _registered = other._registered;
    _passwordCorrect = other._passwordCorrect;
    return *this;
}

Client::~Client() {}

void Client::setPasswordCorrect(bool passwordCorrect)
{
    _passwordCorrect = passwordCorrect;
}

std::string Client::getNickname() const
{
    return _nickname;
}

void Client::setNickname(std::string nick)
{
    _nickname = nick;
}

void Client::setOperator(bool is_op)
{
    _is_op = is_op;
}

std::string Client::getUsername() const
{
    return _username;
}

bool Client::getOperator() const
{
    return _is_op;
}
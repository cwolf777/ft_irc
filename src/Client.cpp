
#include "Client.hpp"

Client::Client() : _fd(-1), _nickname(""), _registered(false), _is_op(false) {}

Client::Client(int fd, std::string nickname, bool is_registerd, bool is_op) : _fd(fd), _nickname(nickname), _registered(is_registerd), _is_op(is_op) {}

Client::Client(const Client &other) : _fd(other._fd), _nickname(other._nickname), _registered(other._registered), _is_op(other._is_op) {}

Client &Client::operator=(const Client &other)
{
    if (this == &other)
        return *this;

    _nickname = other._nickname;
    _is_op = other._is_op;
    _fd = other._fd;
    _registered = other._registered;
    return *this;
}

Client::~Client() {}

bool Client::get_registered()
{
    return _registered;
}
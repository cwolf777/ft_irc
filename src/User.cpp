
#include "User.hpp"

User::User() : _nickname("default"), _is_op(false), _fd(-1) {}

User::User(const std::string &nickname, bool is_op, int fd) : _nickname(nickname), _is_op(is_op), _fd(fd) {}

User::User(const User &other) : _nickname(other._nickname), _is_op(other._is_op), _fd(other._fd) {}

User &User::operator=(const User &other)
{
    if (this == &other)
        return *this;

    _nickname = other._nickname;
    _is_op = other._is_op;
    _fd = other._fd;
    return *this;
}

User::~User() {}
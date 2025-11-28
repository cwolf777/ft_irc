
#include "Client.hpp"

bool checkNickname(const std::string &nick)
{
    // allowed: letters: a-z, A-Z, numbers: 0-9, extras: - [ ] \ ^ { }`
    // firstChar must NOT be extra or number
    // length 1 - 9
    if (nick.empty())
        return false;

    if (!isalpha(nick[0]))
        return false;

    if (nick.length() < 1 || nick.length() > 9)
        return false;

    for (size_t i = 0; i < nick.size(); i++)
    {
        char c = nick[i];

        if (isalnum(c))
            continue;

        if (c == '-' || c == '[' || c == ']' ||
            c == '\\' || c == '`' || c == '^' ||
            c == '{' || c == '}')
            continue;

        return false;
    }

    return true;
}

Client::Client() : _fd(-1), _nickname(""), _registered(false), _is_op(false), _passwordCorrect(false) {}

Client::Client(int fd, std::string nickname, bool is_registerd, bool is_op) : _fd(fd), _registered(is_registerd), _is_op(is_op)
{
    if (!checkNickname(nickname))
        throw std::invalid_argument("Error invalid nickname"); // TODO: better exception
    _nickname = nickname;
}

Client::Client(int fd, std::string nickname, bool is_registerd, bool is_op, bool passwordCorrect) : _fd(fd), _registered(is_registerd), _is_op(is_op), _passwordCorrect(passwordCorrect)
{
    if (!checkNickname(nickname))
        throw std::invalid_argument("Error invalid nickname"); // TODO: better exception
    _nickname = nickname;
}

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

Client::~Client() {} // TODO: CLOSE FD

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

int Client::getFd() const
{
    return _fd;
}

bool Client::getRegistered() const
{
    return _registered;
}

bool Client::getPasswordCorrect() const
{
    return _passwordCorrect;
}

std::ostream &operator<<(std::ostream &os, const Client &client)
{
    os << "=== Client ===" << std::endl;

    os << "  FD (Socket):      " << client.getFd() << std::endl;
    os << "  Nickname:         " << client.getNickname() << std::endl;
    os << "  Username:         " << client.getUsername() << std::endl;

    os << "  Registration:     ";
    if (client.getRegistered())
    {
        os << "**REGISTERED** (Ready)";
    }
    else
    {
        os << "Pending";
    }
    os << std::endl;

    os << "  Operator Status:  ";
    if (client.getOperator())
    {
        os << "Yes (OP)";
    }
    else
    {
        os << "No";
    }
    os << std::endl;

    os << "  Password Correct: ";
    if (client.getPasswordCorrect())
    {
        os << "Validated";
    }
    else
    {
        os << "Not Checked/Failed";
    }
    os << std::endl;

    os << "===================" << std::endl;

    return os;
}
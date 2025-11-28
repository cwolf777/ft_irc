#include "Server.hpp"

std::string Server::getPassword() const
{
    return _password;
}

void Server::setPassword(std::string pass)
{
    _password = pass;
}

bool Server::isNickUsed(const std::string &nick)
{
    for (auto it = _clients.begin(); it != _clients.end(); it++)
    {
        if (it->getNickname() == nick)
            return true;
    }
    return false;
}

const std::string Server::getOperatorPassword() const
{
    return _operatorPassword;
}

bool Server::isUsernameUsed(const std::string &username)
{
    for (auto it = _clients.begin(); it != _clients.end(); it++)
    {
        if (it->getUsername() == username)
            return true;
    }
    return false;
}

std::string Server::getOperatorName() const
{
    return _operatorName;
}

Channel Server::getChannel(const std::string &name) const
{
    for (size_t i = 0; i < _channel.size(); i++)
    {
        if (_channel[i].getName() == name)
            return _channel[i];
    }
    throw ServerException("");
}

Client Server::getClientByNick(const std::string nick) const
{
    for (auto it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->getNickname() == nick)
        {
            return (*it);
        }
    }
    throw ServerException("");
}
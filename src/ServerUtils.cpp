#include "Server.hpp"

// Channel &Server::getChannelByName(const std::string &name)
// {
//     if (_channelMap.find(name) == _channelMap.end())
//         throw ServerException("Channel does not exist");
//     return _channelMap[name];
// }

const Client &Server::getClientByNick(const std::string &nickname) const
{

    for (const std::pair<const int, Client> &pair : _clients)
    {
        if (pair.second.getNickname() == nickname)
            return pair.second;
    }

    throw ServerException("Err: Nickname not found");
}

std::string Server::getPassword() const
{
    return _password;
}

size_t Server::getChannelLimit() const
{
    return _channelLimit;
}

size_t Server::getClientLimit() const
{
    return _clientLimit;
}

void Server::setPassword(std::string pass)
{
    _password = pass; // TODO: password validation
}

bool Server::isNickUsed(const std::string &nick) const
{
    for (std::pair<const int, Client> entry : _clients)
    {
        if (entry.second.getNickname() == nick)
            return true;
    }
    return false;
}

bool Server::isUsernameUsed(const std::string &username) const
{
    for (std::pair<const int, Client> entry : _clients)
    {
        if (entry.second.getUsername() == username)
            return true;
    }
    return false;
}

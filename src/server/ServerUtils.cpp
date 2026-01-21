#include "Server.hpp"

// Channel &Server::getChannelByName(const std::string &name)
// {
//     if (_channelMap.find(name) == _channelMap.end())
//         throw ServerException("Channel does not exist");
//     return _channelMap[name];
// }

std::string Server::getServerName() const
{
    return _serverName;
}

std::string Server::getPassword() const
{
    return _password;
}

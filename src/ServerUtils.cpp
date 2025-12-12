#include "Server.hpp"

bool Server::channelExists(const std::string &name) const
{

    auto it = std::find_if(_channelList.begin(), _channelList.end(),
                           [name](const Channel &c)
                           {
                               return c.getName() == name;
                           });

    if (it == _channelList.end())
        return false;
    return true;
}

Channel &Server::getChannel(const std::string &name)
{
    auto it = std::find_if(_channelList.begin(), _channelList.end(),
                           [name](const Channel &c)
                           {
                               return c.getName() == name;
                           });

    if (it == _channelList.end())
        throw ServerException("Channel does not exist");
    return *it;
}

std::string Server::getPassword() const
{
    return _password;
}

void Server::setPassword(std::string pass)
{
    _password = pass; // TODO: password validation
}

bool Server::isNickUsed(const std::string &nick) const
{
    auto it = std::find_if(_clients.begin(), _clients.end(),
                           [nick](const Client &c)
                           {
                               return c.getNickname() == nick;
                           });

    if (it == _clients.end())
        return false;
    return true;
}

const std::string Server::getOperatorPassword() const
{
    return _operatorPassword;
}

bool Server::isUsernameUsed(const std::string &username) const
{
    auto it = std::find_if(_clients.begin(), _clients.end(),
                           [username](const Client &c)
                           {
                               return c.getUsername() == username;
                           });

    if (it == _clients.end())
        return false;
    return true;
}

std::string Server::getOperatorName() const
{
    return _operatorName;
}

Client &Server::getClientByNick(const std::string &nick)
{
    auto it = std::find_if(_clients.begin(), _clients.end(),
                           [nick](const Client &c)
                           {
                               return c.getNickname() == nick;
                           });

    if (it == _clients.end())
        throw ServerException("No Client with " + nick + " found");
    return *it;
}
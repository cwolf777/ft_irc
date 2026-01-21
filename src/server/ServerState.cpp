#include "ServerState.hpp"

ServerState::ServerState() : _channelLimit(100), _clientLimit(100)
{
}

ServerState::ServerState(const ServerState &other) : _clients(other._clients),
                                                     _channels(other._channels),
                                                     _channelLimit(other._channelLimit),
                                                     _clientLimit(other._clientLimit)
{
}

ServerState &ServerState::operator=(const ServerState &other)
{
    if (this == &other)
        return *this;
    _channelLimit = other._channelLimit;
    _clientLimit = other._clientLimit;
    _clients = other._clients;
    _channels = other._channels;
    return *this;
}

ServerState::~ServerState() {}

Client *ServerState::getClientByNick(const std::string &nick)
{

    for (std::pair<const int, Client> &pair : _clients)
    {
        if (pair.second.getNickname() == nick)
            return &pair.second;
    }

    return nullptr;
}

bool ServerState::isNickUsed(const std::string &nick) const
{
    for (std::pair<const int, Client> entry : _clients)
    {
        if (entry.second.getNickname() == nick)
            return true;
    }
    return false;
}

bool ServerState::isUsernameUsed(const std::string &username) const
{
    for (std::pair<const int, Client> entry : _clients)
    {
        if (entry.second.getUsername() == username)
            return true;
    }
    return false;
}

// TODO: soll ich logik schon einbauen und checken auf duplicates
void ServerState::addClient(Client &client)
{
    _clients[client.getFd()] = client;
}

void ServerState::removeClient(const Client &client)
{
    _clients.erase(client.getFd());
}

std::map<std::string, Channel> &ServerState::getChannels()
{
    return _channels;
}

Channel *ServerState::getChannel(const std::string &name)
{
    for (std::pair<const std::string, Channel> &pair : _channels)
    {
        if (pair.second.getName() == name)
            return &pair.second;
    }
    return nullptr;
}

// TODO: auch hier checken wenn channel schon in _channel existiert
void ServerState::addChannel(Channel &channel)
{
    // wird hier eine kopie von channel in _channels inserted ?
    _channels[channel.getName()] = channel;
}

void ServerState::removeChannel(const Channel &channel)
{
    _channels.erase(channel.getName());
}

size_t ServerState::getChannelLimit() const
{
    return _channelLimit;
}

size_t ServerState::getClientLimit() const
{
    return _clientLimit;
}


#pragma once
#include <map>
#include "Client.hpp"
#include "Channel.hpp"

class ServerState
{
public:
    std::map<int, Client> _clients;
    std::map<std::string, Channel> _channels;
    size_t _channelLimit;
    size_t _clientLimit;

    ServerState();
    ServerState(const ServerState &other);
    ServerState &operator=(const ServerState &other);
    ~ServerState();

    // Clients
    Client *getClientByNick(const std::string &nick);
    bool isNickUsed(const std::string &nick) const;
    bool isUsernameUsed(const std::string &user) const;
    void addClient(Client &client);
    void removeClient(const Client &client);

    // Channels
    std::map<std::string, Channel> &getChannels();
    Channel *getChannel(const std::string &name);
    void addChannel(Channel &channel);
    void removeChannel(const Channel &channel);

    // Limits
    size_t getClientLimit() const;
    size_t getChannelLimit() const;

    class ServerStateException : public std::exception
    {
    private:
        std::string _message;

    public:
        ServerStateException(const std::string &msg) : _message(msg) {}
        const char *what() const noexcept override { return _message.c_str(); }
    };
};
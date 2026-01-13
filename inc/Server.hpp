
#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "IrcMsg.hpp"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <exception>
#include <algorithm>

class Server
{
private:
    std::string _serverName;
    std::string _serverPrefix;
    int _port;
    std::string _password;
    int _server_fd;
    std::map<int, Client> _clients;
    std::map<std::string, Channel> _channels;
    std::vector<pollfd> _poll_fds;
    int _channelLimit;
    int _clientLimit;

public:
    class ServerException : public std::exception
    {
    private:
        std::string _message;

    public:
        ServerException(const std::string &msg) : _message(msg) {}
        const char *what() const noexcept override { return _message.c_str(); }
    };

    Server(const std::string &serverName, int _port, const std::string &_password);
    Server(const Server &other);
    Server &operator=(const Server &other);
    ~Server();

    void init(int domain);
    void run();

    std::string getPassword() const;
    Channel &getChannelByName(const std::string &name);
    std::vector<Channel> getChannelsByTopic(const std::string &topic); // TODO:
    size_t getChannelLimit() const;
    size_t getClientLimit() const;

    void setPassword(std::string pass);

    bool isNickUsed(const std::string &nick) const;
    bool isUsernameUsed(const std::string &username) const;

    void sendResponse(const Client &client, const IrcMsg &response) const;
    void sendResponse(const Client &client, const std::string &msg) const;
    void sendResponse(const Client &client, const char *msg) const;
    void sendWelcomeMessage(const Client &client) const;

    void handleRequest(Client &client, const IrcMsg &msg);
    void handleCap(Client &client, const IrcMsg &msg);
    void handlePass(Client &client, const IrcMsg &msg);
    void handleNick(Client &client, const IrcMsg &msg);
    void handleUser(Client &client, const IrcMsg &msg);
    void handleClient(Client &client, const IrcMsg &msg);
    void handleOper(Client &client, const IrcMsg &msg);
    void handleMode(Client &client, const IrcMsg &msg);
    void handleQuit(Client &client, const IrcMsg &msg);
    void handleJoin(Client &client, const IrcMsg &msg);
    void handleTopic(Client &client, const IrcMsg &msg);
    void handleKick(Client &client, const IrcMsg &msg);
    void privMsg(Client &client, const IrcMsg &msg);
    void handleNotice(Client &client, const IrcMsg &msg);
    void handlePing(Client &client, const IrcMsg &msg);

    void broadcastToChannel(const Client &client, Channel &channel, const std::string &msg);

    void connectClient();
    void addChannel(const Channel &channel);
    void disconnectClient(Client &client);
};
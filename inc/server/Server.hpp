
#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "IrcMsg.hpp"
#include "CommandDispatcher.hpp"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <exception>
#include <algorithm>
#include "ServerState.hpp"

class Server
{
private:
    std::string _serverName;
    int _port;
    std::string _password;
    int _server_fd;
    std::vector<pollfd> _poll_fds;
    CommandDispatcher _commands;
    ServerState _state;

public:
    class ServerException : public std::exception
    {
    private:
        std::string _message;

    public:
        ServerException(const std::string &msg) : _message(msg) {}
        const char *what() const noexcept override { return _message.c_str(); }
    };

    Server(const std::string &serverName, int port, const std::string &password);
    Server(const Server &other);
    Server &operator=(const Server &other);
    ~Server();

    void init(int domain);
    void run();

    ServerState &getServerState();
    std::string getServerName() const;
    std::string getPassword() const;

    // void sendMsg(const Client &client, const IrcMsg &response) const;
    void sendMsg(const Client &client, const std::string &msg) const;
    void sendMsg(const Client &client, const char *msg) const;
    void sendWelcomeMessage(const Client &client) const;

    void handleRequest(Client &client, const IrcMsg &msg);

    void broadcastToChannel(const Client &client, const Channel &channel, const std::string &msg);

    void connectClient();
    void disconnectClient(Client &client);
};
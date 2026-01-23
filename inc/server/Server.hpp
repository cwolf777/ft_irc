#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "IrcMsg.hpp"
#include "CommandDispatcher.hpp"
#include "ServerState.hpp"
#include "ServerException.hpp"

#include <cstring>
#include <csignal>
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

class Server
{
private:
    std::string _serverName;
    int _port;
    std::string _password;
    int _server_fd;
    std::vector<pollfd> _polls;
    CommandDispatcher _commands;
    ServerState _state;

public:
    Server(const std::string &serverName, int port, const std::string &password);
    Server(const Server &other);
    Server &operator=(const Server &other);
    ~Server();

    void init(int domain);
    void run();

    ServerState &getServerState();
    std::string getServerName() const;
    std::string getPassword() const;

    void sendMsg(const Client &client, const std::string &msg) const;
    void sendMsg(const Client &client, const char *msg) const;
    void sendWelcomeMessage(const Client &client) const;
    void broadcastToChannel(const Client &client, const Channel &channel, const std::string &msg);

    bool receive(int fd, std::string &data);
    void processBuffer(Client &client, std::string &buffer);
    void handleRequest(Client &client, const IrcMsg &msg);

    void connectClient();
    void disconnectClient(Client &client);
    void shutdown(const std::string &reason);
};
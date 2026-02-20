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

namespace Color
{
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string RESET = "\033[0m";
}

class Server
{
private:
    std::string _serverName;
    std::string _password;
    std::string _creationDate;
    int _port;
    int _server_fd;
    std::vector<pollfd> _polls;
    CommandDispatcher _commands;
    ServerState _state;

public:
    Server(const std::string &serverName, const std::string &password, int port);
    Server(const Server &other);
    Server &operator=(const Server &other);
    ~Server();

    void init(int domain);
    void run();

    ServerState &getServerState();
    std::string getServerName() const;
    std::string getPassword() const;
    pollfd &getPollByFd(int fd);

    void sendMsg(Client &client, const std::string &msg);
    // void sendMsg(const Client &client, const char *msg) const;
    void sendWelcomeMessage(Client &client);
    void broadcastToChannel(const Client &client, const Channel &channel, const std::string &msg);

    bool receive(int fd, std::string &data);
    void flushClient(Client &client);
    void processData(Client &client, std::string &rawData);
    void handleRequest(Client &client, const IrcMsg &msg);

    void connectClient();
    void disconnectClient(Client &client);
    void shutdown(const std::string &reason);
};
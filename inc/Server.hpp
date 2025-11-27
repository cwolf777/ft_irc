
#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "IrcMsg.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <exception>

class Server
{
private:
    int _port;
    std::string _password;
    int _server_fd;
    std::vector<Client> _clients;
    std::string _operatorName;
    std::string _operatorPassword;
    std::vector<Channel> _channel;
    std::vector<pollfd> _poll_fds;

public:
    class ServerException : public std::exception
    {
    private:
        std::string _message;

    public:
        ServerException(const std::string &msg) : _message(msg) {}
        const char *what() const noexcept override { return _message.c_str(); }
    };

    Server(int _port, std::string _password);
    Server(const Server &other);
    Server &operator=(const Server &other);
    ~Server();

    void init(int domain);
    void run();

    std::string getPassword() const;
    Channel getChannel(const std::string &name) const;
    Client getClientByNick(const std::string nick) const;
    const std::string getOperatorPassword() const;
    std::string getOperatorName() const;

    void setPassword(std::string pass);
    bool isNickUsed(const std::string &nick);
    bool isUsernameUsed(const std::string &username);
    // void handle_msg(std::string msg);
    void send_response(const Client &client, const IrcMsg &response);

    void handleRequest(Client &client, const IrcMsg &msg);
    void handleCap(Client &client, const IrcMsg &msg);
    void handlePass(Client &client, const IrcMsg &msg);
    void handleNick(Client &client, const IrcMsg &msg);
    void handleClient(Client &client, const IrcMsg &msg);
    void handleOper(Client &client, const IrcMsg &msg);
    void handleMode(Client &client, const IrcMsg &msg);
    void handleQuit(Client &client, const IrcMsg &msg);
    void handleJoin(Client &client, const IrcMsg &msg);
    void handleTopic(Client &client, const IrcMsg &msg);
    void handleKick(Client &client, const IrcMsg &msg);
    void privMsg(Client &client, const IrcMsg &msg);
    void handleNotice(Client &client, const IrcMsg &msg);
};
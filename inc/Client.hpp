
#pragma once

#include "IrcMsg.hpp"
#include <poll.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <exception>

class Client
{
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    bool _registered;
    bool _is_op;
    bool _passwordCorrect;

public:
    Client();
    Client(int fd, std::string nickname, bool registered, bool is_op, bool passwordCorrect);
    Client(int fd, std::string nickname, bool is_registerd, bool is_op);
    Client(const Client &other);
    Client &operator=(const Client &other);
    ~Client();

    int getFd() const;
    bool getRegistered() const;
    bool getPasswordCorrect() const;
    std::string getNickname() const;
    std::string getUsername() const;
    bool getOperator() const;
    void setNickname(std::string nick);
    void sendMessage(const std::string &msg);
    void setPasswordCorrect(bool passwordCorrect);
    void setOperator(bool is_op);
};

std::ostream &operator<<(std::ostream &os, const Client &client);

#pragma once

#include "IrcMsg.hpp"
#include <poll.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <exception>
#include <unistd.h>
#include "Channel.hpp"

class Client
{
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    std::string _realName;
    std::string _hostname;

    bool _hasNick;
    bool _hasUser;
    bool _hasPass;
    bool _isRegistered;

    std::vector<Channel> _channels;

public:
    Client();
    Client(int fd);
    Client(int fd, const std::string &hostname);
    Client(const Client &other);
    Client &operator=(const Client &other);
    ~Client();

    int getFd() const;
    std::string getNickname() const;
    std::string getUsername() const;
    std::string getRealname() const;
    bool getIsRegistered() const;
    std::vector<Channel> getChannels() const;
    std::string getPrefix() const;
    bool hasNick() const;
    bool hasUser() const;
    bool hasPass() const;

    void setNickname(const std::string &nick);
    void setUsername(const std::string &name);
    void setRealname(const std::string &name);
    void setHasPass(bool flag);
    void setHasNick(bool flag);
    void setHasUser(bool flag);

    bool canRegister();
};

std::ostream &operator<<(std::ostream &os, const Client &client);
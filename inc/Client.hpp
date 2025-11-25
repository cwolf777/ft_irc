
#pragma once

#include <poll.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

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
    Client(const Client &other);
    Client &operator=(const Client &other);
    ~Client();

    bool get_registered();
    std::string getNickname();
    void setNickname(std::string nick);
    void sendMessage(const std::string &msg);
    void setPasswordCorrect(bool passwordCorrect);
    void setOperator(bool is_op);
    std::string getUsername();
    bool getOperator();
};

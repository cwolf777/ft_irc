
#pragma once

#include <string>

class User
{
private:
    std::string _nickname;
    bool _is_op;
    int _fd;
    bool _registered; //in OCF eintragen

public:
    User();
    User(const std::string &nickname, bool is_op, int fd);
    User(const User &other);
    User &operator=(const User &other);
    ~User();

    void sendMessage(const std::string &msg);
    bool isRegistered();
    void setRegistration(bool isRegistered);
    void setNickname(std::string nickname);
};
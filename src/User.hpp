
#pragma once

#include <string>

class User
{
private:
    std::string _nickname;
    bool _is_op;
    int _fd;

public:
    User();
    User(const std::string &nickname, bool is_op, int fd);
    User(const User &other);
    User &operator=(const User &other);
    ~User();
};
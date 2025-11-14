#include "Channel.hpp"

Channel::Channel() : _name(""), _topic(""), _inviteOnly(false), _topicProtected(false), _password(""), _userlimit(INT32_MAX){}

Channel::Channel(const Channel &other) : _name(other._name), _topic(other._topic), _memberList(other._memberList), _operatorList(other._operatorList), _inviteOnly(other._inviteOnly), _topicProtected(other._topicProtected), _password(other._password), _userlimit(other._userlimit) {}

Channel &Channel::operator=(const Channel &other)
{
    if (this == &other)
        return *this;

    _name = other._name;
    _topic = other._topic;
    _memberList = other._memberList;
    _operatorList = other._operatorList;

    _inviteOnly = other._inviteOnly;
    _topicProtected = other._topicProtected;
    _password = other._password;
    _userlimit = other._userlimit;
    return *this;
}

Channel::~Channel() {}
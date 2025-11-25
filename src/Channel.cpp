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

std::string Channel::getName()
{
    return _name;
}

bool Channel::isMember(const std::string &name)
{
    for (auto it = _memberList.begin(); it != _memberList.end(); ++it)
    {
        if (it->getNickname() == name)
            return true;
    }
    return false;
}

void Channel::setInviteOnly(bool inviteOnly)
{
    _inviteOnly = inviteOnly;
}

void Channel::setTopicProtected(bool topicProtected)
{
    _topicProtected = topicProtected;
}

void Channel::clearPassword()
{
    _passwordSet = false;
}

void Channel::setPassword(const std::string password)
{
    _password = password;
}

void Channel::clearUserLimit()
{
    _userLimitSet = false;
}

void Channel::setLimit(int limit)
{
    _userlimit = limit;
}
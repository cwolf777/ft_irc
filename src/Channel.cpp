#include "Channel.hpp"

Channel::Channel() : _name(""), _topic(""), _inviteOnly(false), _topicProtected(false), _password(""), _userlimit(INT32_MAX) {}

Channel::Channel(const std::string &name, const std::string &password) : _name(name), _topic(""), _password(password)
{
    // TODO: password check channel name check topic check
}

Channel::Channel(const Channel &other) : _name(other._name), _topic(other._topic), _memberList(other._memberList), _operatorList(other._operatorList), _inviteOnly(other._inviteOnly), _topicProtected(other._topicProtected), _password(other._password), _userlimit(other._userlimit)
{
}

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

std::string Channel::getName() const
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

void Channel::addOperator(Client &client)
{
    for (auto it = _operatorList.begin(); it != _operatorList.end(); ++it)
    {
        if (it->getNickname() == client.getNickname())
            return;
    }
    _operatorList.push_back(client);
}

// void Channel::removeOperator(Client &client)
// {
//     for (auto it = _operatorList.begin(); it != _operatorList.end(); ++it)
//     {
//         if (it->getNickname() == client.getNickname())
//         {
//             _operatorList.erase(it);
//             return;
//         }
//         return;
//     }
// }

bool Channel::isOperator(Client &client)
{
    for (auto it = _operatorList.begin(); it != _operatorList.end(); ++it)
    {
        if (it->getNickname() == client.getNickname())
        {
            return true;
        }
    }
    return false;
}

bool Channel::isUserLimitSet()
{
    if (_userLimitSet == true)
        return true;
    else
        return false;
}

void Channel::removeFromList(std::vector<Client> &list, const std::string &nick)
{
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        if (it->getNickname() == nick)
        {
            list.erase(it);
            return;
        }
    }
}

void Channel::removeMember(Client &client)
{
    removeFromList(_memberList, client.getNickname());
}

void Channel::removeOperator(Client &client)
{
    removeFromList(_operatorList, client.getNickname());
}

bool Channel::isPasswordSet() const
{
    return _passwordSet;
}

std::string Channel::getPassword() const
{
    return _password;
}

size_t Channel::getMemberCount() const
{
    return _memberList.size();
}

size_t Channel::getUserLimit() const
{
    return _userlimit;
}

std::string Channel::getTopic() const
{
    return _topic;
}

void Channel::setTopic(std::string topic)
{
    _topic = topic;
}

bool Channel::isTopicProtected() const
{
    return _topicProtected;
}
const std::vector<Client> &Channel::getMembers() const
{
    return _memberList;
}

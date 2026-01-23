#include "Channel.hpp"
#include "Client.hpp"

// TODO: warum userlimitSet false wenn userlimit 100 ist ???
Channel::Channel()
    : _name(""),
      _topic(""),
      _password(""),
      _inviteOnly(false),
      _topicProtected(false),
      _passwordSet(false),
      _userLimitSet(false),
      _userlimit(100) {}

Channel::Channel(const std::string &name)
    : _name(name),
      _topic(""),
      _password(""),
      _inviteOnly(false),
      _topicProtected(false),
      _passwordSet(false),
      _userLimitSet(false),
      _userlimit(100)
{
    // TODO: password check channel name check topic check
}
Channel::Channel(const std::string &name, const std::string &password)
    : _name(name),
      _topic(""),
      _password(password),
      _inviteOnly(false),
      _topicProtected(false),
      _passwordSet(true),
      _userLimitSet(false),
      _userlimit(100)
{
    // TODO: password check channel name check topic check
}

Channel::Channel(const Channel &other)
    : _name(other._name),
      _topic(other._topic),
      _password(other._password),
      _members(other._members),
      _operators(other._operators),
      _invites(other._invites),
      _inviteOnly(other._inviteOnly),
      _topicProtected(other._topicProtected),
      _passwordSet(other._passwordSet),
      _userLimitSet(other._userLimitSet),
      _userlimit(other._userlimit)
{
}

Channel &Channel::operator=(const Channel &other)
{
    if (this == &other)
        return *this;

    _name = other._name;
    _topic = other._topic;
    _password = other._password;
    _members = other._members;
    _operators = other._operators;
    _invites = other._invites;

    _inviteOnly = other._inviteOnly;
    _topicProtected = other._topicProtected;
    _passwordSet = other._passwordSet;
    _userLimitSet = other._userLimitSet;
    _userlimit = other._userlimit;
    return *this;
}

Channel::~Channel() {}

bool Channel::isMember(const std::string &name) const
{

    for (Client *c : _members)
    {
        if (c->getNickname() == name)
            return true;
    }
    return false;
}

bool Channel::isMember(const Client &client) const
{

    for (Client *c : _members)
    {
        if (c->getNickname() == client.getNickname())
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
    _password = "";
}

void Channel::setPassword(const std::string password)
{
    _passwordSet = true;
    _password = password;
}

void Channel::clearUserLimit()
{
    _userLimitSet = false;
    _userlimit = 100;
}

void Channel::setLimit(int limit)
{
    _userLimitSet = true;
    _userlimit = limit;
}

void Channel::addOperator(Client *oper)
{
    for (Client *c : _operators)
    {
        if (c->getNickname() == oper->getNickname())
            return; // TODO: EXCEPTION
    }
    _operators.push_back(oper);
}

void Channel::addMember(Client *member)
{
    for (Client *c : _members)
    {
        if (c->getNickname() == member->getNickname())
            return; // TODO: EXCEPTION
    }
    _members.push_back(member);
}

void Channel::invite(Client *client)
{
    _invites.insert(client);
}

bool Channel::isOperator(const Client &client) const
{
    for (Client *c : _operators)
    {
        if (c->getNickname() == client.getNickname())
            return true;
    }
    return false;
}

bool Channel::isUserLimitSet() const
{
    return _userLimitSet;
}

void Channel::removeMember(const Client &client)
{
    for (auto it = _members.begin(); it != _members.end(); ++it)
    {
        if ((*it)->getNickname() == client.getNickname())
        {
            _members.erase(it);
            return;
        }
    }
}

bool Channel::isPasswordSet() const
{
    return _passwordSet;
}

bool Channel::isInviteOnly() const
{
    return _inviteOnly;
}

bool Channel::isInvited(const Client &client) const
{
    for (Client *c : _invites)
    {
        if (client.getFd() == c->getFd())
            return true;
    }
    return false;
}

std::string Channel::getName() const
{
    return _name;
}

std::string Channel::getTopic() const
{
    return _topic;
}

std::string Channel::getPassword() const
{
    return _password;
}

size_t Channel::getUserLimit() const
{
    return _userlimit;
}

const std::vector<Client *> &Channel::getMembers() const
{
    return _members;
}

const std::vector<Client *> &Channel::getOperators() const
{
    return _operators;
}

bool Channel::isTopicProtected() const
{
    return _topicProtected;
}

void Channel::setTopic(std::string newTopic)
{
    _topic = newTopic;
}

std::ostream &operator<<(std::ostream &os, const Channel &channel)
{
    os << "========== CHANNEL INFO ==========" << "\n"
       << "Name:           " << channel.getName() << "\n"
       << "Topic:          " << (channel.getTopic().empty() ? "(no topic)" : channel.getTopic()) << "\n"
       << "----------------------------------" << "\n"
       << "Modes:" << "\n"
       << "  Invite-Only:  " << (channel.isInviteOnly() ? "Yes" : "No") << "\n"
       << "  Password:     " << (channel.isPasswordSet() ? ("Set (" + channel.getPassword() + ")") : "No") << "\n"
       << "  User-Limit:   " << (channel.isUserLimitSet() ? std::to_string(channel.getUserLimit()) : "No") << "\n"
       << "----------------------------------" << "\n"
       << "Members (" << channel.getMembers().size() << "):" << "\n";

    if (channel.getMembers().empty())
    {
        os << "  (none)\n";
    }
    else
    {
        for (Client *cl : channel.getMembers())
        {
            bool isOp = false;
            for (Client *op : channel.getOperators())
            {
                if (op == cl)
                {
                    isOp = true;
                    break;
                }
            }
            os << "  " << (isOp ? "@" : " ") << cl->getNickname() << " (FD: " << cl->getFd() << ")\n";
        }
    }
    os << "==================================";
    return os;
}
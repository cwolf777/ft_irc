
#pragma once

#include <string>
#include <vector>
#include <functional>

class Client;

class Channel
{
private:
    std::string _name;
    std::string _topic;
    std::vector<Client *> _members;
    std::vector<Client *> _operators;
    std::vector<Client *> _invites;

    // verscheidene Modes....
    bool _inviteOnly;
    bool _topicProtected;
    bool _passwordSet;
    std::string _password;
    bool _userLimitSet;
    int _userlimit;

public:
    Channel();
    Channel(const std::string &name);
    Channel(const std::string &name, const std::string &password);
    Channel(const Channel &other);
    Channel &operator=(const Channel &other);
    ~Channel();

    void addOperator(Client *client);
    void addMember(Client *client);

    void removeMember(const Client &client);

    std::string getName() const;
    std::string getTopic() const;
    std::string getPassword() const;
    const std::vector<Client *> &getMembers() const;
    const std::vector<Client *> &getOperators() const;
    size_t getUserLimit() const;

    bool isMember(const std::string &name);
    bool isOperator(const Client &client) const;
    bool isPasswordSet() const;
    bool isTopicProtected() const;
    bool isUserLimitSet();
    bool isInviteOnly() const;
    bool isInvited(const Client &client) const;

    void setInviteOnly(bool inviteOnly);
    void setTopicProtected(bool topicProtected);
    void setPassword(const std::string password);
    void setTopic(std::string newTopic);
    void setLimit(int limit);

    void clearPassword();
    void clearUserLimit();
};

std::ostream &operator<<(std::ostream &os, const Channel &channel);

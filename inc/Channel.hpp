
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

    std::string getName() const;
    bool isMember(const std::string &name);
    void setInviteOnly(bool inviteOnly);
    void setTopicProtected(bool topicProtected);
    void clearPassword();
    void setPassword(const std::string password);
    std::string getPassword() const;
    bool isPasswordSet() const;
    void clearUserLimit();
    void setLimit(int limit);
    bool isUserLimitSet();
    bool isOperator(const Client &client) const;

    void addOperator(Client *client);
    void addMember(Client *client);

    void removeMember(const Client &client);
    const std::vector<Client *> &getMembers() const;
    size_t getMemberCount() const;
    size_t getUserLimit() const;

    void broadcastMessage(const std::string &msg) const;
};
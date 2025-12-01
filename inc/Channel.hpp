#include <string>
#include <vector>
#include "Client.hpp"

class Channel
{
private:
    std::string _name;
    std::string _topic;
    std::vector<Client> _memberList;
    std::vector<Client> _operatorList;

    // verscheidene Modes....
    bool _inviteOnly;
    bool _topicProtected;   
    bool _passwordSet;
    std::string _password;
    bool _userLimitSet;
    int _userlimit;

public:
    Channel();
    Channel(const std::string &nickname, bool is_op, int fd);
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
    void addOperator(Client &client);
    void removeOperator(Client &client);
    bool isOperator(Client &client);
    void removeFromList(std::vector<Client> &list, const std::string &nick);
    void removeMember(Client &client);
    void removeOperator(Client &client);
    const std::vector<Client*>& getMembers() const;
    size_t getMemberCount() const;
    size_t getUserLimit() const;
    std::string getTopic() const;
    void setTopic(std::string topic);
    bool isTopicProtected() const;
};
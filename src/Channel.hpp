#include <string>
#include <vector>
#include "User.hpp"

class Channel
{
private:
    std::string _name;
    std::string _topic;
    std::vector<User> _memberList;
    std::vector<User> _operatorList;

    //verscheidene Modes....
    bool _inviteOnly;
    bool _topicProtected;
    std::string _password;
    int _userlimit;
public:
    Channel();
    Channel(const std::string &nickname, bool is_op, int fd);
    Channel(const Channel &other);
    Channel &operator=(const Channel &other);
    ~Channel();
};
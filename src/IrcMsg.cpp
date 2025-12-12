
#include "IrcMsg.hpp"

IrcMsg::IrcMsg() : _cmd(""), _org_msg("") {}

bool is_valid_cmd(const std::string &cmd)
{
    for (auto it = IRC_COMMANDS.begin(); it < IRC_COMMANDS.end(); it++)
    {
        if (*it == cmd)
            return true;
    }
    return false;
}

IrcMsg::IrcMsg(const std::string &msg) : _org_msg(msg)
{
    create(msg);
}

IrcMsg::IrcMsg(const char *msg) : _org_msg(msg)
{
    create(msg);
}

IrcMsg::IrcMsg(const IrcMsg &other) : _prefix(other._prefix), _cmd(other._cmd), _params(other._params), _org_msg(other._org_msg) {}

IrcMsg &IrcMsg::operator=(const IrcMsg &other)
{
    if (this == &other)
        return *this;
    _prefix = other._prefix;
    _cmd = other._cmd;
    _params = other._params;
    _org_msg = other._org_msg;
    return *this;
}

IrcMsg::~IrcMsg() {};

void IrcMsg::create(const std::string &msg)
{

    if (msg.size() < 2 || msg.substr(msg.size() - 2) != "\r\n")
        throw IrcMsgException("Message not terminated properly with \\r\\n");

    _org_msg = msg.substr(0, msg.size() - 2);
    std::stringstream ss(_org_msg);
    std::string token;
    ss >> token;

    if (token[0] == ':')
    {
        _prefix = token.substr(1);
        ss >> token;
    }

    if (!is_valid_cmd(token))
        throw InvalidCommand();

    _cmd = token;
    int middle_params = 0;
    while (ss >> token)
    {
        if (token[0] == ':')
        {
            // trailing argument
            std::string remainder;
            std::getline(ss, remainder);
            _params.push_back(token.substr(1) + remainder);
            break;
        }
        _params.push_back(token);
        middle_params++;

        if (middle_params > 14) // RFC: max 14 middle params + 1 trailing
            throw TooManyParams();
    }
}

std::string IrcMsg::get_cmd(void) const
{
    return _cmd;
}

std::string IrcMsg::get_msg(void) const
{
    return _org_msg;
}

std::vector<std::string> IrcMsg::get_params(void) const
{
    return _params;
}

std::string IrcMsg::get_prefix(void) const
{
    return _prefix;
}

std::ostream &operator<<(std::ostream &os, const IrcMsg &msg)
{
    os << "=== RequestMsg ===" << std::endl;

    os << "Original: " << msg.get_msg() << "\n";

    if (!msg.get_prefix().empty())
        os << "Prefix:   " << msg.get_prefix() << "\n";
    else
        os << "Prefix:   <none>" << std::endl;

    os << "Command:  " << msg.get_cmd() << "\n";

    std::vector<std::string> params(msg.get_params());

    os << "Params:   ";
    if (params.empty())
    {
        os << "<none>" << std::endl;
    }
    else
    {
        for (size_t i = 0; i < params.size(); ++i)
        {
            os << "[" << i << "]=\"" << params[i] << "\"";
            if (i + 1 < params.size())
                os << ", ";
        }
        os << std::endl;
    }

    os << "===================" << std::endl;

    return os;
}
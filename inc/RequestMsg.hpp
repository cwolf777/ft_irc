
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

#define IRC_CAP "CAP"
#define IRC_PASS "PASS"
#define IRC_NICK "NICK"
#define IRC_USER "USER"
#define IRC_OPER "OPER"
#define IRC_MODE "MODE"
#define IRC_QUIT "QUIT"
#define IRC_JOIN "JOIN"
#define IRC_TOPIC "TOPIC"
#define IRC_KICK "KICK"
#define IRC_PRIVMSG "PRIVMSG"
#define IRC_NOTICE "NOTICE"
#define IRC_UNKNOWN "UNKOWN"

enum IRC_CMD
{
    CAP,
    PASS,
    NICK,
    USER,
    OPER,
    MODE,
    QUIT,
    JOIN,
    TOPIC,
    KICK,
    PRIVMSG,
    NOTICE,
    UNKNOWN
};

static const std::unordered_map<std::string, IRC_CMD> IRC_COMMANDS = {
    {IRC_CAP, CAP},
    {IRC_PASS, PASS},
    {IRC_NICK, NICK},
    {IRC_USER, USER},
    {IRC_OPER, OPER},
    {IRC_MODE, MODE},
    {IRC_QUIT, QUIT},
    {IRC_JOIN, JOIN},
    {IRC_TOPIC, TOPIC},
    {IRC_KICK, KICK},
    {IRC_PRIVMSG, PRIVMSG},
    {IRC_NOTICE, NOTICE},
    {IRC_UNKNOWN, UNKNOWN}};

class RequestMsg
{
private:
    std::string _cmd;
    std::vector<std::string> _args;
    std::string _org_msg;

public:
    RequestMsg();
    RequestMsg(std::string msg);
    RequestMsg(const RequestMsg &other);
    RequestMsg &operator=(const RequestMsg &other);
    ~RequestMsg();
};
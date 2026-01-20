
#pragma once

#include <ostream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <array>

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
#define IRC_INVITE "INVITE"
#define IRC_PRIVMSG "PRIVMSG"
#define IRC_NOTICE "NOTICE"
#define IRC_PING "PING"
#define IRC_NAMES "NAMES"
#define IRC_WHO "WHO"
#define IRC_PART "PART"

static const std::array<std::string, 17> IRC_COMMANDS = {
    IRC_CAP,
    IRC_PASS,
    IRC_NICK,
    IRC_USER,
    IRC_OPER,
    IRC_MODE,
    IRC_QUIT,
    IRC_JOIN,
    IRC_TOPIC,
    IRC_INVITE,
    IRC_KICK,
    IRC_PRIVMSG,
    IRC_NOTICE,
    IRC_PING,
    IRC_NAMES,
    IRC_WHO,
    IRC_PART};

class IrcMsg
{
private:
    std::string _prefix;
    std::string _cmd;
    std::vector<std::string> _params;
    std::string _org_msg;

public:
    class IrcMsgException : public std::exception
    {
    private:
        std::string _message;

    public:
        IrcMsgException(const std::string &msg) : _message(msg) {}
        const char *what() const noexcept override { return _message.c_str(); }
    };

    class InvalidCommand : public IrcMsgException
    {
    public:
        InvalidCommand() : IrcMsgException("Invalid IRC Command") {}
    };

    class TooManyParams : public IrcMsgException
    {
    public:
        TooManyParams() : IrcMsgException("Too many parameters in IRC message") {}
    };

    IrcMsg();
    IrcMsg(const std::string &msg);
    IrcMsg(const char *msg);
    IrcMsg(const IrcMsg &other);
    IrcMsg &operator=(const IrcMsg &other);
    ~IrcMsg();

    void create(const std::string &msg);
    std::string get_cmd(void) const;
    std::string get_msg(void) const;
    std::vector<std::string> get_params(void) const;
    std::string get_prefix(void) const;
};

std::ostream &operator<<(std::ostream &os, const IrcMsg &msg);
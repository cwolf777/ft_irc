#pragma once
#include <map>
#include "handler.hpp"

class Server;
typedef void (*handlerFunc)(Client &, Server &, const IrcMsg &);

// TODO: canonical fertig machen
class CommandDispatcher
{
private:
    std::map<std::string, handlerFunc> _commands;

public:
    CommandDispatcher();
    void registerCommand(const std::string &name, handlerFunc cmd);

    void dispatch(Client &client, Server &server, const IrcMsg &msg);
};

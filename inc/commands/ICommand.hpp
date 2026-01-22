#pragma once
#include "Client.hpp"
#include "IrcMsg.hpp"

class Server;

class ICommand
{
public:
    virtual ~ICommand() {}
    virtual void execute(Client &client, Server &server, const IrcMsg &msg) = 0;
};

#pragma once
#include "ICommand.hpp"

class PrivMsgCommand : public ICommand
{
public:
    void execute(Client &client, Server &server, const IrcMsg &msg) override;
};
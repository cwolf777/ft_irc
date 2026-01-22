
#pragma once
#include "ICommand.hpp"

class QuitCommand : public ICommand
{
public:
    void execute(Client &client, Server &server, const IrcMsg &msg) override;
};
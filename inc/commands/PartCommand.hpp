
#pragma once
#include "ICommand.hpp"

class PartCommand : public ICommand
{
public:
    void execute(Client &client, Server &server, const IrcMsg &msg) override;
};
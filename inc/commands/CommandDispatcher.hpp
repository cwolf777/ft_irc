#pragma once
#include <map>
#include <memory>
#include "Command.hpp"
#include "CapCommand.hpp"
#include "InviteCommand.hpp"
#include "JoinCommand.hpp"
#include "KickCommand.hpp"
#include "ModeCommand.hpp"
#include "NamesCommand.hpp"
#include "PassCommand.hpp"
#include "NickCommand.hpp"
#include "PingCommand.hpp"
#include "PrivMsgCommand.hpp"
#include "QuitCommand.hpp"
#include "TopicCommand.hpp"
#include "UserCommand.hpp"
#include "WhoCommand.hpp"
#include "PartCommand.hpp"

// TODO: canonical fertig machen
class CommandDispatcher
{
private:
    std::map<std::string, std::unique_ptr<Command>> _commands;

public:
    CommandDispatcher();
    void registerCommand(const std::string &name,
                         std::unique_ptr<Command> cmd);

    void dispatch(Client &client,
                  Server &server,
                  const IrcMsg &msg);
};

#include "CommandDispatcher.hpp"

CommandDispatcher::CommandDispatcher()
{
    registerCommand("JOIN", handleJoin);
    registerCommand("NICK", handleNick);
    registerCommand("PRIVMSG", handlePrivMsg);
    registerCommand("CAP", handleCap);
    registerCommand("INVITE", handleInvite);
    registerCommand("KICK", handleKick);
    registerCommand("MODE", handleMode);
    registerCommand("NAMES", handleNames);
    registerCommand("PASS", handlePass);
    registerCommand("PING", handlePing);
    registerCommand("PRIVMSG", handlePrivMsg);
    registerCommand("QUIT", handleQuit);
    registerCommand("TOPIC", handleTopic);
    registerCommand("USER", handleUser);
    registerCommand("WHO", handleWho);
    registerCommand("PART", handlePart);
}

void CommandDispatcher::registerCommand(const std::string &name, handlerFunc cmd)
{
    _commands[name] = cmd;
}

void CommandDispatcher::dispatch(Client &client, Server &server, const IrcMsg &msg)
{
    std::string cmd = msg.get_cmd();

    auto it = _commands.find(cmd);
    if (it == _commands.end())
    {
        // ERR_UNKNOWNCOMMAND
        std::cout << "Unkown Command" << std::endl;
        return;
    }

    it->second(client, server, msg);
}

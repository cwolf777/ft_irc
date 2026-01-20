#include "CommandDispatcher.hpp"

CommandDispatcher::CommandDispatcher()
{
    registerCommand("JOIN", std::make_unique<JoinCommand>());
    registerCommand("NICK", std::make_unique<NickCommand>());
    registerCommand("PRIVMSG", std::make_unique<PrivMsgCommand>());
    registerCommand("CAP", std::make_unique<CapCommand>());
    registerCommand("INVITE", std::make_unique<InviteCommand>());
    registerCommand("KICK", std::make_unique<KickCommand>());
    registerCommand("MODE", std::make_unique<ModeCommand>());
    registerCommand("NAMES", std::make_unique<NamesCommand>());
    registerCommand("PASS", std::make_unique<PassCommand>());
    registerCommand("PING", std::make_unique<PingCommand>());
    registerCommand("PRIVMSG", std::make_unique<PrivMsgCommand>());
    registerCommand("QUIT", std::make_unique<QuitCommand>());
    registerCommand("TOPIC", std::make_unique<TopicCommand>());
    registerCommand("USER", std::make_unique<UserCommand>());
    registerCommand("WHO", std::make_unique<WhoCommand>());
    registerCommand("PART", std::make_unique<PartCommand>());
}

void CommandDispatcher::registerCommand(const std::string &name,
                                        std::unique_ptr<Command> cmd)
{
    _commands[name] = std::move(cmd);
}

void CommandDispatcher::dispatch(Client &client,
                                 Server &server,
                                 const IrcMsg &msg)
{
    std::string cmd = msg.get_cmd();

    auto it = _commands.find(cmd);
    if (it == _commands.end())
    {
        // ERR_UNKNOWNCOMMAND
        std::cout << "Unkown Command" << std::endl;
        return;
    }

    it->second->execute(client, server, msg);
}

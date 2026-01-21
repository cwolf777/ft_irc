#include "NickCommand.hpp"
#include "Server.hpp"
#include "Channel.hpp"

void NickCommand::execute(Client &client, Server &server, const IrcMsg &msg)
{
    if (msg.get_params().empty())
    {
        // Format: :<servername> 431 <current_nick> :No nickname given
        std::string reply = ":" + server.getServerName() + " 431 " + client.getNickname() + " :No nickname given\r\n";
        server.sendMsg(client, reply);
        return;
    }

    std::string newNickname = msg.get_params()[0];
    ServerState &state = server.getServerState();
    if (state.isNickUsed(newNickname))
    {
        // Format: :<servername> 433 <current_nick> <target_nick> :Nickname is already in use
        std::string currentNick = client.hasNick() ? client.getNickname() : "*";
        std::string reply = ":" + server.getServerName() + " 433 " + currentNick + " " + newNickname + " :Nickname is already in use\r\n";
        server.sendMsg(client, reply);
        return;
    }

    try
    {
        std::string oldPrefix = client.getPrefix();
        std::string oldNick = client.getNickname();

        client.setNickname(newNickname);
        client.setHasNick(true);

        if (client.getIsRegistered())
        {
            std::string reply = ":" + oldPrefix + " NICK :" + newNickname + "\r\n";
            server.sendMsg(client, reply);
            for (Channel *chan : client.getChannels())
            {
                server.broadcastToChannel(client, *chan, reply);
            }
        }
        if (client.canRegister())
            server.sendWelcomeMessage(client);
    }
    catch (const std::exception &e)
    {
        // 432: Erroneous nickname
        std::string reply = ":" + server.getServerName() + " 432 * " + newNickname + " :Erroneous nickname\r\n";
        server.sendMsg(client, reply);
    }
}
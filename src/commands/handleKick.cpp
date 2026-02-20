#include "Server.hpp"
#include "Channel.hpp"
#include "ServerState.hpp"

void handleKick(Client &client, Server &server, const IrcMsg &msg)
{
    // KICK <channel> <user> [comment]
    if (msg.get_params().size() < 2)
    {
        std::string reply = ":" + server.getServerName() + " 461 " + client.getNickname() + " KICK :Not enough parameters\r\n";
        server.sendMsg(client, reply);
        return;
    }

    ServerState &state = server.getServerState();
    const std::string channelName = msg.get_params()[0];
    const std::string targetNick = msg.get_params()[1];

    std::string comment = client.getNickname();

    if (msg.get_params().size() >= 3)
    {
        comment.clear();
        for (size_t i = 2; i < msg.get_params().size(); i++)
        {
            comment += msg.get_params()[i];
            if (i + 1 < msg.get_params().size())
                comment += " ";
        }
    }

    Channel *currChannel = state.getChannel(channelName);
    if (!currChannel)
    {
        std::string reply = ":" + server.getServerName() + " 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
        server.sendMsg(client, reply);
        return;
    }

    if (!currChannel->isMember(client))
    {
        std::string reply = ":" + server.getServerName() + " 442 " + client.getNickname() + " " + channelName + " :You're not on that channel\r\n";
        server.sendMsg(client, reply);
        return;
    }

    if (!currChannel->isOperator(client))
    {
        //  THIS IS COMMENTED OUT BECAUSE OF IRSSI
        //  std::string reply = ":" + server.getServerName() + " 482 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n";
        //  server.sendMsg(client, reply);
        return;
    }

    Client *target = state.getClientByNick(targetNick);
    if (!target)
    {
        std::string reply = ":" + server.getServerName() + " 441 " + client.getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
        server.sendMsg(client, reply);
        return;
    }

    // User exist but not in the target Channel
    if (!currChannel->isMember(*target))
    {
        std::string reply = ":" + server.getServerName() + " 441 " + client.getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
        server.sendMsg(client, reply);
        return;
    }

    std::string kickMsg = ":" + client.getPrefix() + " KICK " + channelName + " " + targetNick + " :" + comment + "\r\n";

    server.broadcastToChannel(client, *currChannel, kickMsg);
    server.sendMsg(client, kickMsg);
    server.sendMsg(*target, kickMsg);

    target->leaveChannel(currChannel);

    if (currChannel->isOperator(*target))
        currChannel->removeOperator(*target);

    currChannel->removeMember(*target);
    if (currChannel->getMembers().empty())
        state.removeChannel(*currChannel);
}
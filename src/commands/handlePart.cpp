#include "Server.hpp"
#include "Channel.hpp"

void handlePart(Client &client, Server &server, const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();

    if (params.size() < 1)
    {
        std::string reply = ":" + server.getServerName() + " 461 " + client.getNickname() + " PART :Not enough parameters\r\n";
        server.sendMsg(client, reply);
        return;
    }
    ServerState &state = server.getServerState();
    std::string reason = "";
    if (params.size() > 1)
        reason = params[1];

    std::stringstream ss(params[0]);
    std::string currChannelName;

    while (std::getline(ss, currChannelName, ','))
    {

        Channel *currChannel = state.getChannel(currChannelName);
        if (!currChannel)
        {
            std::string reply = ":" + server.getServerName() + " 403 " + client.getNickname() + " " + currChannelName + " :No such channel\r\n";
            server.sendMsg(client, reply);
            continue;
        }

        if (!currChannel->isMember(client.getNickname()))
        {
            std::string reply = ":" + server.getServerName() + " 442 " + client.getNickname() + " " + currChannelName + " :You're not on that channel\r\n";
            server.sendMsg(client, reply);
            continue;
        }

        std::string reply = ":" + client.getPrefix() + " PART " + currChannelName;

        if (!reason.empty())
            reply += " :" + reason;

        reply += "\r\n";
        server.broadcastToChannel(client, *currChannel, reply);
        server.sendMsg(client, reply);

        if (currChannel->isOperator(client))
            currChannel->removeOperator(client);

        currChannel->removeMember(client);

        if (currChannel->getMembers().empty())
            state.removeChannel(*currChannel);
    }
}
#include "PartCommand.hpp"
#include "Server.hpp"
#include "Channel.hpp"

void PartCommand::execute(Client &client,
                          Server &server,
                          const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();

    if (params.size() < 1)
    {
        server.sendResponse(client,
                            ":" + server.getServerName() + " 461 " + client.getNickname() +
                                " PART :Not enough parameters\r\n");
        return;
    }

    std::string reason;
    if (params.size() > 1)
    {
        reason = params[1];
    }

    std::stringstream ss(params[0]);
    std::string channelName;

    while (std::getline(ss, channelName, ','))
    {

        if (server.getChannels().find(channelName) == server.getChannels().end())
        {
            server.sendResponse(client,
                                ":" + server.getServerName() + " 403 " + client.getNickname() +
                                    " " + channelName + " :No such channel\r\n");
            continue;
        }

        Channel &channel = server.getChannels()[channelName];

        if (!channel.isMember(client.getNickname()))
        {
            server.sendResponse(client,
                                ":" + server.getServerName() + " 442 " + client.getNickname() +
                                    " " + channelName + " :You're not on that channel\r\n");
            continue;
        }

        std::string response =
            ":" + client.getPrefix() + " PART " + channelName;

        if (!reason.empty())
            response += " :" + reason;

        response += "\r\n";

        server.broadcastToChannel(client, channel, response);
        server.sendResponse(client, response);

        channel.removeMember(client);

        if (channel.getMembers().empty())
            server.getChannels().erase(channelName);
    }
}
#include "TopicCommand.hpp"
#include "Server.hpp"
#include "Channel.hpp"

void TopicCommand::execute(Client &client, Server &server, const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();

    if (params.size() < 1)
    {
        server.sendMsg(client, ":" + server.getServerName() + " 461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n");
        return;
    }

    const std::string &channelName = params[0];
    ServerState &state = server.getServerState();
    Channel *channel = state.getChannel(channelName);

    if (!channel)
    {
        server.sendMsg(client, ":" + server.getServerName() + " 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
        return;
    }

    if (!channel->isMember(client.getNickname()))
    {
        server.sendMsg(client, ":" + server.getServerName() + " 442 " + client.getNickname() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }

    if (params.size() == 1)
    {
        if (channel->getTopic().empty())
            server.sendMsg(client, ":" + server.getServerName() + " 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n");
        else
            server.sendMsg(client, ":" + server.getServerName() + " 332 " + client.getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n");
        return;
    }

    if (channel->isTopicProtected() && !channel->isOperator(client))
    {
        server.sendMsg(client, ":" + server.getServerName() + " 482 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }

    std::string newTopic = params[1];

    channel->setTopic(newTopic);
    std::string response = ":" + client.getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    server.broadcastToChannel(client, *channel, response);
    server.sendMsg(client, response);
}
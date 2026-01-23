#include "Server.hpp"
#include "Channel.hpp"
#include "ServerState.hpp"

// Command: INVITE
// Parameters : <nickname> <channel>
void handleInvite(Client &client, Server &server, const IrcMsg &msg)
{
    std::vector<std::string> inviteParams = msg.get_params();

    // if (inviteParams.size() != 2)
    // {
    //     return;
    // }
    ServerState &state = server.getServerState();
    const std::string &nickname = inviteParams[0];
    const std::string &channelName = inviteParams[1];

    Client *invitedClient = state.getClientByNick(nickname);
    // 401 ERR_NOSUCHNICK
    if (!invitedClient)
    {
        std::string reply = ":" + server.getServerName() + " 401 " + client.getNickname() + " " + nickname + " :No such nick/Name\r\n";
        server.sendMsg(client, reply);
        return;
    }

    // 403 ERR_NOSUCHCHANNEL
    Channel *channel = state.getChannel(channelName);
    if (!channel)
    {
        std::string reply = ":" + server.getServerName() + " 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
        server.sendMsg(client, reply);
        return;
    }
    // 442 ERR_NOTONCHANNEL
    if (!channel->isMember(client))
    {
        std::string reply = ":" + server.getServerName() + " 442 " + client.getNickname() + " " + channelName + " :You're not on that channel\r\n";
        server.sendMsg(client, reply);
        return;
    }

    // 443 ERR_USERONCHANNEL
    // TODO: maybe check if invitedCLient is operator
    if (channel->isMember(*invitedClient))
    {
        std::string reply = ":" + server.getServerName() + " 443 " + client.getNickname() + " " + nickname + " " + channelName + " :is already on channel\r\n";
        server.sendMsg(client, reply);
        return;
    }

    // 482 ERR_CHANOPRIVSNEEDED
    if (!channel->isOperator(client))
    {
        std::string reply = ":" + server.getServerName() + " 443 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n";
        server.sendMsg(client, reply);
        return;
    }
    channel->invite(invitedClient);
    // 341 RPL_INVITING
    std::string reply = ":" + server.getServerName() + " 341 " + client.getNickname() + " " + nickname + " " + channelName + "\r\n";
    server.sendMsg(client, reply);

    reply = ":" + client.getPrefix() + " INVITE " + nickname + " :" + channelName + "\r\n";
    server.sendMsg(*invitedClient, reply);
}
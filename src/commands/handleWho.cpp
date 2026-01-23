#include "Server.hpp"
#include "Channel.hpp"
#include "ServerState.hpp"

void handleWho(Client &client, Server &server, const IrcMsg &msg)
{
    if (msg.get_params().empty())
        return;

    std::string channelName = msg.get_params()[0];
    ServerState &state = server.getServerState();
    if (channelName[0] == '#' || channelName[0] == '&')
    {

        Channel *chan = state.getChannel(channelName);
        if (!chan)
        {
            server.sendMsg(client, ":" + server.getServerName() + " 315 " + client.getNickname() + " " + channelName + " :End of WHO list\r\n");
            return;
        }
        for (Client *member : chan->getMembers())
        {
            // 352 RPL_WHOREPLY
            std::string reply = ":" + server.getServerName() + " 352 " + client.getNickname() + " " + channelName + " " + member->getUsername() + " " + member->getHostname() + " " + server.getServerName() + " " + member->getNickname() + " H :0 " + member->getRealname() + "\r\n";
            server.sendMsg(client, reply);
        }
    }
    server.sendMsg(client, ":" + server.getServerName() + " 315 " + client.getNickname() + " " + channelName + " :End of WHO list\r\n");
}
#include "Server.hpp"
#include "Channel.hpp"

// NOTICE <target> :<text>
void handleNotice(Client &client, Server &server, const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();

    // darf keine Errors erzeugen
    if (params.size() < 2)
        return;
    ServerState &state = server.getServerState();
    std::string target = params[0];
    std::string text = params[1];

    if (target[0] == '#')
    {
        Channel *channel = state.getChannel(target);
        if (!channel)
            return;

        if (!channel->isMember(client))
            return;
        server.broadcastToChannel(client, *channel,
                                  ":" + client.getPrefix() + " NOTICE " + target + " :" + text + "\r\n");
    }
    else
    {
        Client *receiver = state.getClientByNick(target);
        if (!receiver)
            return;

        server.sendMsg(*receiver,
                       ":" + client.getPrefix() + " NOTICE " + target + " :" + text + "\r\n");
    }
}
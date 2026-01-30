#include "Server.hpp"
#include "Channel.hpp"

void handleNames(Client &client, Server &server, const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();
    if (params.empty())
        return;

    ServerState &state = server.getServerState();
    const std::string &channelName = params[0];

    Channel *currChannel = state.getChannel(channelName);

    if (!currChannel)
    {
        std::string reply = ":" + server.getServerName() + " 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
        server.sendMsg(client, reply);
        return; // ERR_NOSUCHCHANNEL
    }

    std::string names;
    for (const Client *member : currChannel->getMembers())
    {
        if (currChannel->isOperator(*member))
            names += "@";

        names += member->getNickname();
        names += " ";
    }

    // 3. 353 RPL_NAMREPLY senden
    std::string reply = ":" + server.getServerName() + " 353 " + client.getNickname() + " = " + channelName + " :" + names + "\r\n";
    server.sendMsg(client, reply);

    // 4. 366 RPL_ENDOFNAMES senden
    std::string end = ":" + server.getServerName() + " 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list.\r\n";
    server.sendMsg(client, end);
}
#include "Server.hpp"
#include "Channel.hpp"

void handleQuit(Client &client, Server &server, const IrcMsg &msg)
{
    std::string reason = msg.get_params().empty() ? client.getNickname() : msg.get_params()[0];

    std::string reply = ":" + client.getPrefix() + " QUIT :Quit: " + reason + "\r\n";

    // TODO: REMOVE FROM OPERATOR LIST
    for (Channel *chan : client.getChannels())
    {
        server.broadcastToChannel(client, *chan, reply);
        chan->removeMember(client);
    }

    reply = "ERROR :Closing Link: " + client.getHostname() + " (Quit: " + reason + ")\r\n";
    server.sendMsg(client, reply);
    server.disconnectClient(client);
}
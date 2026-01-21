#include "PingCommand.hpp"
#include "Server.hpp"
#include "Channel.hpp"

void PingCommand::execute(Client &client, Server &server, const IrcMsg &msg)
{
    if (msg.get_params().empty())
    {
        server.sendMsg(client, "PONG :" + server.getServerName() + "\r\n");
        return;
    }

    std::string pingParam = msg.get_params()[0];
    server.sendMsg(client, "PONG :" + pingParam + "\r\n");
}
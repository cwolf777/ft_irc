#include "PassCommand.hpp"
#include "Server.hpp"
#include "Channel.hpp"

void PassCommand::execute(Client &client, Server &server, const IrcMsg &msg)
{
    if (client.getIsRegistered())
    {
        server.sendMsg(client, ":" + server.getServerName() + " 462 " + client.getNickname() + " :You may not reregister\r\n");
        return;
    }
    if (msg.get_params().size() < 1)
    {
        server.sendMsg(client, ":" + server.getServerName() + "461 PASS :Not enough parameters\r\n");
        return;
    }
    std::string pass = msg.get_params()[0];

    if (pass != server.getPassword())
    {
        server.sendMsg(client, ":" + server.getServerName() + "464 * :Password incorrect\r\n");
        server.sendMsg(client, "ERROR :Closing Link: [Access denied by password]\r\n");
        throw Server::ServerException("Auth failed");
        return;
    }
    client.setHasPass(true);
    if (client.canRegister())
        server.sendWelcomeMessage(client);
}
#include "Server.hpp"
#include "Channel.hpp"

void handlePass(Client &client, Server &server, const IrcMsg &msg)
{
    if (client.hasPass())
    {
        server.sendMsg(client, ":" + server.getServerName() + " 462 " + client.getNickname() + " :You may not reregister\r\n");
        return;
    }

    if (msg.get_params().empty())
    {
        server.sendMsg(client, ":" + server.getServerName() + " 461 * PASS :Not enough parameters\r\n");
        return;
    }

    std::string pass = msg.get_params()[0];

    if (pass != server.getPassword())
    {
        // 464 ERR_PASSWDMISMATCH
        server.sendMsg(client, ":" + server.getServerName() + " 464 * :Password incorrect\r\n");
        server.sendMsg(client, "ERROR :Closing Link: Access denied (incorrect password)\r\n");
        server.disconnectClient(client);
        return;
    }

    client.setHasPass(true);
    if (client.canRegister())
    {
        server.sendWelcomeMessage(client);
    }
}
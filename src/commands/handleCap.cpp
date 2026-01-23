#include "Server.hpp"
#include "Channel.hpp"

void handleCap(Client &client, Server &server, const IrcMsg &msg)
{
    if (client.getIsRegistered())
    {
        std::cout << "Warning: Registered client sent CAP command. Ignoring." << std::endl;
        return;
    }

    if (msg.get_params().size() < 1)
    {

        // TODO: CAP needs params
        return;
    }

    if (msg.get_params()[0] == "LS")
    {
        server.sendMsg(client, "CAP * LS\r\n");
        return;
    }

    if (msg.get_params()[0] == "END" && client.canRegister())
        server.sendWelcomeMessage(client);
}
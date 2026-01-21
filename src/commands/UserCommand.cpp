#include "UserCommand.hpp"
#include "Server.hpp"
#include "Channel.hpp"

void UserCommand::execute(Client &client, Server &server, const IrcMsg &msg)
{
    // USER <username> <hostname> <servername> :<realname>
    // USER chris 0 * :Christopher Klein
    //-> hostname und serverbname ignoriert da eh schon bekannt
    std::vector<std::string> params = msg.get_params();
    if (params.size() < 4)
    {
        server.sendMsg(client, "432 :Erroneous nickname\r\n");
        throw Server::ServerException("432 :Erroneous nickname"); // TODO: really exception ??
    }

    // TODO: Check for already existing username

    client.setUsername(params[1]); // TODO: Check for valid Username and real name
    client.setRealname(params[3]);
    client.setHasUser(true);
    if (client.canRegister())
        server.sendWelcomeMessage(client);
}
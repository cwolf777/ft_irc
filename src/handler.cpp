
#include "Server.hpp"

void Server::handleRequest(Client &client, const IrcMsg &msg)
{

    const std::string cmd = msg.get_cmd();

    void (Server::*functions[])(Client &, const IrcMsg &) = {
        &Server::handleCap,
        &Server::handlePass,
        &Server::handleNick,
        &Server::handleUser,
        // &Server::handleClient,
        // &Server::handleOper,
        // &Server::handleQuit,
        // &Server::handleJoin,
        // &Server::handleTopic,
        // &Server::handleKick,
        // &Server::privMsg,
        // &Server::handleNotice,
    };

    for (size_t i = 0; i < IRC_COMMANDS.size(); i++)
    {
        if (cmd == IRC_COMMANDS[i])
        {
            (this->*functions[i])(client, msg);
            return;
        }
    }
    throw ServerException("Invalid Cmd");
}

void Server::handleCap(Client &client, const IrcMsg &msg)
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
        sendResponse(client, "CAP * LS\r\n");
        return;
    }

    if (msg.get_params()[0] == "END")
    {
        if (!client.hasNick())
        {
            sendResponse(client, "451 * :You have not registered\r\n");
            throw ServerException("451 * :You have not registered\r\n");
        }
        if (!client.hasUser())
        {
            sendResponse(client, "451 * :You have not registered\r\n");
            throw ServerException("451 * :You have not registered\r\n");
        }
        if (!client.hasPass())
        {
            sendResponse(client, "451 * :You have not registered, Password required\r\n");
            throw ServerException("451 * :You have not registered, Password required\r\n");
        }
        if (client.canRegister())
            sendWelcomeMessage(client);

        return;
    }
}

void Server::handlePass(Client &client, const IrcMsg &msg)
{
    if (client.getIsRegistered())
    {
        sendResponse(client, "462 :You may not register\r\n");
        return;
    }
    if (msg.get_params().size() < 1)
    {
        sendResponse(client, "461 PASS :Not enough parameters\r\n");
        return;
    }
    std::string pass = msg.get_params()[0];

    if (pass != getPassword())
    {
        sendResponse(client, "464 :Password incorrect\r\n");

        // sendResponse(client, "ERROR :Closing Link: user@host [Password Incorrect]\r\n");
        // disconnectClient(client);
        // TODO: WELCHE ERROR MESSAGE ???????????
        return;
    }
    client.setHasPass(true);
    // if (client.canRegister())
    //     sendWelcomeMessage(client);
}

void Server::handleNick(Client &client, const IrcMsg &msg)
{
    if (msg.get_params().size() < 1)
    {
        sendResponse(client, "431 :No nickname given\r\n");
        throw ServerException("431 :No nickname given");
    }
    std::string newNickname = msg.get_params()[0];
    if (isNickUsed(newNickname))
    {
        sendResponse(client, "433 :Nickname is already in use\r\n");
        throw ServerException("433 :Nickname is already in use");
    }

    try
    {
        client.setNickname(newNickname);
        client.setHasNick(true);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        sendResponse(client, "432 :Erroneous nickname\r\n");
        throw Server::ServerException("432 :Erroneous nickname");
    }

    // Nachricht an alle Clients ausser beim ersten setzen dbeim einloggen:
    //: NICK <oldNick> <newNick>
}

void Server::handleUser(Client &client, const IrcMsg &msg)
{
    // USER <username> <hostname> <servername> :<realname>
    // USER chris 0 * :Christopher Klein
    //-> hostname und serverbname ignoriert da eh schon bekannt
    std::vector<std::string> params = msg.get_params();
    if (params.size() < 4)
    {
        sendResponse(client, "432 :Erroneous nickname\r\n");
        throw Server::ServerException("432 :Erroneous nickname");
    }

    // if (client.hasUser())
    // {
    //     sendResponse(client, "462 :You may not reregister\r\n");
    //     return;
    // }

    client.setUsername(params[1]); // TODO: Check for valid Username and real name
    client.setRealname(params[4]);
    client.setHasUser(true);
}
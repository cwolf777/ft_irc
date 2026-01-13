
#include "Server.hpp"

void Server::handleRequest(Client &client, const IrcMsg &msg)
{

    const std::string cmd = msg.get_cmd();

    using CommandFunction = void (Server::*)(Client &, const IrcMsg &);

    static const std::map<std::string, CommandFunction> functions{

        {"CAP", &Server::handleCap},
        {"PASS", &Server::handlePass},
        {"NICK", &Server::handleNick},
        {"USER", &Server::handleUser},

        // {"OPER", &Server::handleOper},
        {"MODE", &Server::handleMode},
        {"QUIT", &Server::handleQuit},

        {"JOIN", &Server::handleJoin},
        // {"TOPIC", &Server::handleTopic},
        // {"KICK", &Server::handleKick},

        // {"PRIVMSG", &Server::privMsg},
        // {"NOTICE", &Server::handleNotice}
        {"PING", &Server::handlePing}};

    auto it = functions.find(cmd);
    if (it != functions.end())
    {
        CommandFunction handler = it->second;
        (this->*handler)(client, msg);
    }
    else
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
        // if (!client.hasNick())
        // {
        //     sendResponse(client, "451 * :You have not registered\r\n");
        //     throw ServerException("451 * :You have not registered\r\n");
        // }
        // if (!client.hasUser())
        // {
        //     sendResponse(client, "451 * :You have not registered\r\n");
        //     throw ServerException("451 * :You have not registered\r\n");
        // }
        // if (!client.hasPass())
        // {
        //     sendResponse(client, "451 * :You have not registered, Password required\r\n");
        //     throw ServerException("451 * :You have not registered, Password required\r\n");
        // }
        if (client.canRegister())
            sendWelcomeMessage(client);

        return;
    }
}

void Server::handlePass(Client &client, const IrcMsg &msg)
{
    if (client.getIsRegistered())
    {
        sendResponse(client, ":" + _serverName + " 462 " + client.getNickname() + " :You may not reregister\r\n");
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
        sendResponse(client, _serverPrefix + "464 * :Password incorrect\r\n");
        sendResponse(client, "ERROR :Closing Link: [Access denied by password]\r\n");
        throw ServerException("Auth failed");
        return;
    }
    client.setHasPass(true);
    if (client.canRegister())
        sendWelcomeMessage(client);
}

void Server::handleNick(Client &client, const IrcMsg &msg)
{
    if (msg.get_params().empty())
    {
        // Format: :<servername> 431 <current_nick> :No nickname given
        std::string reply = ":" + _serverName + " 431 " + client.getNickname() + " :No nickname given\r\n";
        sendResponse(client, reply);
        return;
    }

    std::string newNickname = msg.get_params()[0];

    if (isNickUsed(newNickname))
    {
        // Format: :<servername> 433 <current_nick> <target_nick> :Nickname is already in use

        std::string currentNick = client.hasNick() ? client.getNickname() : "*";
        std::string reply = ":" + _serverName + " 433 " + currentNick + " " + newNickname + " :Nickname is already in use\r\n";

        sendResponse(client, reply);
        return;
    }

    try
    {
        std::string oldPrefix = client.getPrefix();
        std::string oldNick = client.getNickname();

        client.setNickname(newNickname);
        client.setHasNick(true);

        if (client.getIsRegistered())
        {
            std::string notify = ":" + oldPrefix + " NICK :" + newNickname + "\r\n";
            // An den Client selbst senden
            sendResponse(client, notify);
            for (Channel *chan : client.getChannels())
            {
                broadcastToChannel(client, *chan, notify);
            }
        }
        if (client.canRegister())
            sendWelcomeMessage(client);
    }
    catch (const std::exception &e)
    {
        // 432: Erroneous nickname
        std::string reply = ":" + _serverName + " 432 * " + newNickname + " :Erroneous nickname\r\n";
        sendResponse(client, reply);
    }
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

    // TODO: Check for already existing username

    std::cout << "username: " << params[1] << " realname: " << params[3] << std::endl;
    client.setUsername(params[1]); // TODO: Check for valid Username and real name
    client.setRealname(params[3]);
    client.setHasUser(true);
    if (client.canRegister())
        sendWelcomeMessage(client);
}

void Server::handleMode(Client &client, const IrcMsg &msg)
{
    std::cout << client << std::endl;
    std::cout << msg << std::endl;
}

void Server::handleQuit(Client &client, const IrcMsg &msg)
{
    std::string reason = msg.get_params().empty() ? client.getNickname() : msg.get_params()[0];

    std::string quitNotify = ":" + client.getPrefix() + " QUIT :Quit: " + reason + "\r\n";

    // TODO: REMOVE FROM OPERATOR LIST
    for (Channel *chan : client.getChannels())
    {
        broadcastToChannel(client, *chan, quitNotify);
        chan->removeMember(client);
    }

    std::string errDoc = "ERROR :Closing Link: " + client.getHostname() + " (Quit: " + reason + ")\r\n";
    sendResponse(client, errDoc);

    disconnectClient(client);
}

void Server::handleJoin(Client &client, const IrcMsg &msg)
{
    // ERR_NEEDMOREPARAMS Join command needs channel name as parameter : JOIN #foobar
    if (msg.get_params().empty())
    {
        std::string reply = ":" + _serverName + " 461 " + client.getNickname() + "JOIN :Not enough parameters\r\n";
        sendResponse(client, reply);
        return;
    }

    std::vector<std::string> joinList, passwordList;
    std::string segment;

    // splitting the channels from params
    std::stringstream ss(msg.get_params()[0]);
    while (std::getline(ss, segment, ','))
    {
        if (!segment.empty())
            joinList.push_back(segment);
    }
    segment.clear();

    // splitting the keys from params
    if (msg.get_params().size() > 1)
    {
        std::stringstream ssPass(msg.get_params()[1]);
        while (std::getline(ssPass, segment, ','))
        {
            passwordList.push_back(segment);
        }
    }
    // loop through joinList to join each channel
    for (int i = 0; i < joinList.size(); i++)
    {
        std::string currChannelName = joinList[i];
        std::string currPass = (i < passwordList.size()) ? passwordList[i] : "";

        // 403 ERR_NOSUCHCHANNEL Channel currChannelName must start with # or &
        if (currChannelName.empty() || (currChannelName[0] != '&' && currChannelName[0] != '#'))
        {
            std::string reply = ":" + _serverName + " 403 " + client.getNickname() + " " + currChannelName + " :No such channel\r\n";
            sendResponse(client, reply);
            continue;
        }

        //  if channel dont exist => create channel
        if (_channels.find(currChannelName) == _channels.end())
        {
            Channel newChannel(currChannelName);
            newChannel.addOperator(&client); // first user becomes admin
            _channels[currChannelName] = newChannel;
        }

        Channel &chan = _channels[currChannelName];

        // 405 ERR_TOOMANYCHANNELS  check server Channellimit
        if (client.getChannels().size() >= getChannelLimit())
        {
            sendResponse(client, ":" + _serverName + " 405 " + client.getNickname() + " " + currChannelName + " :You have joined too many channels\r\n");
            return;
        }
        // 471 ERR_CHANNELISFULL check if channel is full
        if (chan.isUserLimitSet() && chan.getMembers().size() >= chan.getUserLimit())
        {
            std::string reply = ":" + _serverName + " 471 " + client.getNickname() + " " + currChannelName + " :Cannot join channel (+l)\r\n";
            sendResponse(client, reply);
            continue;
        }
        // 475 ERR_BADCHANNELKEY check if password is set and correct
        if (chan.isPasswordSet() && chan.getPassword() != currPass)
        {
            std::string reply = ":" + _serverName + " 475 " + client.getNickname() + " " + currChannelName + " :Cannot join channel (+k)\r\n";
            sendResponse(client, reply);
            continue;
        }
        // add client
        chan.addMember(&client);
        client.joinChannel(&chan);

        std::cout << chan << std::endl;
        // send to everyone in channel a message
        std::string joinMsg = ":" + client.getPrefix() + " JOIN :" + currChannelName + "\r\n";
        broadcastToChannel(client, chan, joinMsg);

        // send client the topic of the channel he joined
        IrcMsg msg("TOPIC " + currChannelName + "\r\n");
        handleTopic(client, msg);
        // sendResponse(client, ":" + _serverName + " 353 " + client.getNickname() + " = " + currChannelName + " :" + chan.getMemberListAsString() + "\r\n");
        // TODO: send a list of names in the channel (RPL_NAMREPLY 353)
        //  sendResponse(client, ":" + _serverName + " 366 " + client.getNickname() + " " + chanName + " :End of /NAMES list\r\n");
    }
}

void Server::handlePing(Client &client, const IrcMsg &msg)
{
    if (msg.get_params().empty())
    {
        sendResponse(client, "PONG :" + _serverName + "\r\n");
        return;
    }

    std::string pingParam = msg.get_params()[0];
    sendResponse(client, "PONG :" + pingParam + "\r\n");
}
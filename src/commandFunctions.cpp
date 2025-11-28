#include "Client.hpp"
#include "Server.hpp"
#include "commandUtils.hpp"

void Server::handleCap(Client &client, const IrcMsg &msg)
{

}

void Server::handlePass(Client &client, const IrcMsg &msg)
{
    if (client.getRegistered())
    {
        client.sendMessage("462 :You may not reregister");
        return;
    }
    if (msg.get_params().size() < 2)
    {
        client.sendMessage("461 PASS :Not enough parameters");
        return;
    }
    std::string pass = msg.get_params()[0];

    if (pass != getPassword())
    {
        client.sendMessage("464 :Password incorrect");
        return;
    }
    client.setPasswordCorrect(true);
}

void Server::handleNick(Client &client, const IrcMsg &msg)
{
    if (msg.get_params().size() < 2)
    {
        client.sendMessage("431 :No nickname given");
        return;
    }
    if (!checkNickname(msg.get_params()[1]))
    {
        client.sendMessage("432 :Erroneous nickname");
        return;
    }

    std::string newNickname = msg.get_params()[0];

    if (isNickUsed(newNickname))
    {
        client.sendMessage("433 :Nickname is already in use");
        return;
    }

    client.setNickname(newNickname);

    // Nachricht an alle Clients ausser beim ersten setzen dbeim einloggen:
    //: NICK <oldNick> <newNick>
}

void Server::handleUser(Client &client, const IrcMsg &msg)
{
    // USER <username> <hostname> <servername> :<realname>
    // USER chris 0 * :Christopher Klein
    //-> hostname und serverbname ignoriert da eh schon bekannt

    // chekcen ob client bereits regestriert
    // chekcen ob genug paramenter
    // daten speichern (username etc)
    // client regestrien setRegistration(true)
    // username != nickname
}

void Server::handleOper(Client &client, const IrcMsg &msg)
{
    // OPER <username> <password>             macht Client zu SERVER OPERATOR...

    if (msg.get_params().size() < 3)
    {
        client.sendMessage("461 OPER :Not enough parameters");
        return;
    }
    std::string operatorName = msg.get_params()[0];
    std::string password = msg.get_params()[1];
    if (operatorName != getOperatorName())
    {
        client.sendMessage("491 :No O-lines for your host");
        return;
    }
    if (getOperatorPassword() != password)
    {
        client.sendMessage("464 :Password incorrect");
        return;
    }
    client.setOperator(true);
    client.sendMessage("381 :You are now an IRC operator");
}

void Server::handleMode(Client &client, const IrcMsg &msg)
{
    // MODE <channel> <modes> [parameters]
    // cechlken ob channel exestiert
    // checkebn ob client in channel ist
    // checken ob client operator ist

    if ( msg.get_params().size() < 2)
    {
        client.sendMessage("461 MODE :Not enough parameters");
        return;
    }

    std::string channelName =  msg.get_params()[0];
    Channel &channel = getChannel(channelName);
    //exception catchen falls kein channel gefunden wurde !!!!!!!!!!!!!!!!!!!!!!!!!

    
    if (!channel.isMember(client.getNickname()))
    {
        client.sendMessage("442 " + channelName + " :You're not on that channel");
        return;
    }

    // nur MODE <channel> ignoriere ich erstmal  gib aktuelle Einstellungen aus

    if (!channel.isOperator(client))
    {
        client.sendMessage("482 " + channelName + " :You're not channel operator");
        return;
    }

    std::string modeString = msg.get_params()[1];
    bool adding = true;
    size_t paramIndex = 3;

    // broadcast string
    std::string appliedModes;
    std::vector<std::string> appliedParams;

    for (size_t i = 0; i < modeString.size(); i++)
    {
        char m = modeString[i];

        if (m == '+')
        {
            adding = true;
            continue;
        }
        if (m == '-')
        {
            adding = false;
            continue;
        }

        switch (m)
        {
        case 'i':
            channel.setInviteOnly(adding);
            appliedModes += (adding ? "+i" : "-i");
            break;

        case 't':
            channel.setTopicProtected(adding);
            appliedModes += (adding ? "+t" : "-t");
            break;

        case 'k':
            if (adding == true)
            {
                if (paramIndex >= msg.get_params().size())
                {
                    client.sendMessage("461 MODE :Not enough parameters");
                    return;
                }
                channel.setPassword(msg.get_params()[paramIndex]);
                appliedModes += "+k";
                appliedParams.push_back(msg.get_params()[paramIndex]);
                paramIndex++;
            }
            else
            {
                channel.clearPassword();
                appliedModes += "-k";
            }
            break;

        case 'l':
            if (adding)
            {
                if (paramIndex >= msg.get_params().size())
                {
                    client.sendMessage("461 MODE :Not enough parameters");
                    return;
                }
                size_t limit = atoi(msg.get_params()[paramIndex].c_str());
                if (limit <= 0)
                {
                    client.sendMessage("461 MODE :Limit must be > 0");
                    return;
                }
                channel.setLimit(limit);
                appliedModes += "+l";
                appliedParams.push_back(msg.get_params()[paramIndex]);
                paramIndex++;
            }
            else
            {
                if (channel.isUserLimitSet() == true)
                    appliedModes += "-l";
                channel.clearUserLimit();
            }
            break;

        case 'o':
            {
            if (paramIndex >= msg.get_params().size())
            {
                client.sendMessage("461 MODE :Not enough parameters");
                return;
            }
            std::string nick = msg.get_params()[paramIndex++];
            Client &target = getClientByNick(nick);

            // if (!target || !channel->isMember(nick)) ANSTATT NLLPOINTER ABFANGEN MUSS ICH EXCEPTION CATCHEN ABER DAS IM ODER NICHT VERGESSEN (!channel->isMember(nick))
            // {
            //     client.sendMessage("441 " + nick + " " + channelName + " :They aren't on that channel");
            //     return;
            // }  
            if (adding)
                channel.addOperator(target);
            else
                channel.removeOperator(target);

            appliedModes += (adding ? "+o" : "-o");
            appliedParams.push_back(nick);

            break;
            }
        default:
            client.sendMessage("472 " + std::string(1, m) + " :is unknown mode char");
            break;
        }
    }

    if (!appliedModes.empty())
    {
        std::string msg = ":" + client.getNickname() + " MODE " + channelName + " " + appliedModes;

        for (size_t i = 0; i < appliedParams.size(); i++)
            msg += " " + appliedParams[i];

        // for (Client* member : channel->getMembers())
        //     member->sendMessage(msg);      //msg an JEDEN clienten der im Channel ist
    }
}

void Server::handleQuit(Client &client, const IrcMsg &msg)
{
    std::string quitMsg = "Client Quit";
    if (!msg.get_params().empty())
        quitMsg == msg.get_params()[0];
    
    std::string fullQuitMsg = ":" + client.getNickname() + "QUIT :" + quitMsg;

    for (Channel &chan : _channel)
    {
        if (chan.isMember(client.getNickname()))
        {
            broadcastToChannel(client, chan, fullQuitMsg);
            chan.removeMember(client);
            chan.removeOperator(client);
        }
    }
    client.sendMessage("ERROR :" + quitMsg);

    //close client FDs
    //remove from poll FDs
}


void Server::handleJoin(Client &client, const IrcMsg &msg)
{
    // check ob arg groesser als 2
    // channle name muss mit # anfangen?
    // get Channel class falls nein erstelle neuen Channel als class
    // add client to channel
    // boradcast join

    if (msg.get_params().empty())
    {
        client.sendMessage("461 MODE :Not enough parameters");
        return;    
    }
    std::string channelName = msg.get_params()[0];
    std::string pass = (msg.get_params().size() > 1) ? msg.get_params()[1] : "";
    if (channelName[0] != '#')
    {
        client.sendMessage("403 " + channelName + " :No such channel");
        return ;
    }
    Channel* channel = nullptr;
    if (!channelExists(channelName)) //no channel -> gets created
    {
        // _channel.push_back(Channel(channelName)); To Do: Channel Contructer fixen
        channel = &_channel.back();

        //First User must be Operator
        channel->addOperator(client);
    }
    else
    {
        channel = &getChannel(channelName); //??? warum &
    }
    //if channel, check if pass correct 
    if (channel->isPasswordSet() && channel->getPassword() != pass)
    {
        client.sendMessage("475 " + channelName + " :Bad channel key");
        return;
    }

    if (channel->isUserLimitSet() && channel->getMemberCount() >= channel->getUserLimit())
    {
        client.sendMessage("471 " + channelName + " :Channel is full");
        return;
    }

    //checken ob bereits member
    //user hinzufuegen
    //braoccast to channel
    //topic an client senden wenn topic vorhanden 
    //memberliste sendne? 

}

void Server::handleTopic(Client &client, const IrcMsg &msg)
{
    // arg muss groeser gleich 2 sein
    //  arg[1] = channel name
    // get channel object if not available return
    // if arg == 2
    //-> braodcast channel name or no channel topic if nothing set yet
    // if arg == 3 topic setzen (in channel struct) ((setter und getter func benutzen))
}

void Server::handleKick(Client &client, const IrcMsg &msg)
{
    // arg muss groesser gleich 3 sein
    // arg[1] channel name
    // arg[2] targetUser
    // get Channel if not return
    // check if client (which want to kick) in channel isMember()
    // check if Client is operaator in channel
    // getUser (target)
    // chekc if target is in channel
    // Grund zusammensetzen mit arg 3, etc falls mehr args
    // Target aus Channel entfernen
}

void Server::privMsg(Client &client, const IrcMsg &msg)
{
    // arg muss groesser gleich 3 sein
    // arg[1] channel name
    // arg[2] targetUser
    // get Channel if not return
    // check if client (which want to kick) in channel isMember()
    // check if Client is operaator in channel
    // getUser (target)
    // chekc if target is in channel
    // Grund zusammensetzen mit arg 3, etc falls mehr args
    // Target aus Channel entfernen
}

void Server::handleNotice(Client &client, const IrcMsg &msg)
{
}

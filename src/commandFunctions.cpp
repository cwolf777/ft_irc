#include "commandFunctions.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "commandUtils.hpp"

bool checkNickname(const std::string &nick);

void handlePass(Server &server, Client &client, const std::vector<std::string> &args)
{
    if (client.get_registered())
    {
        client.sendMessage("462 :You may not reregister");
        return;
    }
    if (args.size() < 2)
    {
        client.sendMessage("461 PASS :Not enough parameters");
        return;
    }
    std::string pass = args[1];

    if (pass != server.getPassword())
    {
      client.sendMessage("464 :Password incorrect");
      return;
    }
    client.setPasswordCorrect(true);
}

void handleNick(Server &server, Client &client, const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        client.sendMessage("431 :No nickname given");
        return;
    }
    if (!checkNickname(args[1]))
    {
        client.sendMessage("432 :Erroneous nickname");
        return;
    }
    
    std::string newNickname = args[1];

    if (server.isNickUsed(newNickname))
    {
        client.sendMessage("433 :Nickname is already in use");
        return;        
    }

    client.setNickname(newNickname);

    //Nachricht an alle Clients ausser beim ersten setzen dbeim einloggen:
    //:NICK <oldNick> <newNick>
}

void handleUser(Client &client, const std::vector<std::string> &args)
{
    // USER <username> <hostname> <servername> :<realname>
    //USER chris 0 * :Christopher Klein
    //-> hostname und serverbname ignoriert da eh schon bekannt
    

    // chekcen ob client bereits regestriert
    //chekcen ob genug paramenter
    //daten speichern (username etc)
    //client regestrien setRegistration(true)
    //username != nickname
}

void handleOper(Server &server, Client &client, const std::vector<std::string> &args)
{
    // OPER <username> <password>             macht Client zu SERVER OPERATOR...

    if (args.size() < 3)
    {
        client.sendMessage("461 OPER :Not enough parameters");
        return;
    }
    std::string operatorName = args[1];
    std::string password = args[2];
    if (operatorName != server.getOperatorName())
    {
        client.sendMessage("491 :No O-lines for your host");
        return;
    }
    if(server.getOperatorPassword() != password)
    {
        client.sendMessage("464 :Password incorrect");
        return;        
    }
    client.setOperator(true);
    client.sendMessage("381 :You are now an IRC operator");
}

void handleMode(Server &server, Client &client, const std::vector<std::string> &args)
{
    // MODE <channel> <modes> [parameters]
    //cechlken ob channel exestiert 
    //checkebn ob client in channel ist 
    //checken ob client operator ist 

    if (args.size() < 2)
    {
        client.sendMessage("461 MODE :Not enough parameters");
        return;
    }

    
    std::string channelName = args[1];
    Channel *channel = server.getChannel(channelName);

    if (!channel)
    {
        client.sendMessage("403 " + channelName + " :No such channel");
        return;
    }
    if (!channel->isMember(client.getNickname()))
    {
        client.sendMessage("442 " + channelName + " :You're not on that channel");
        return;
    }

    //nur MODE <channel> ignoriere ich erstmal  gib aktuelle Einstellungen aus

    if (!channel->isOperator(client))
    {
        client.sendMessage("482 " + channelName + " :You're not channel operator");
        return;
    }

    std::string modeString = args[2];
    bool adding = true;
    size_t paramIndex = 3;

    //broadcast string
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
                channel->setInviteOnly(adding);
                appliedModes += (adding ? "+i" : "-i");
                break;

            case 't':
                channel->setTopicProtected(adding);
                appliedModes += (adding ? "+t" : "-t");
                break;

            case 'k':
                if (adding == true)
                {
                    if (paramIndex >= args.size())
                    {
                        client.sendMessage("461 MODE :Not enough parameters");
                        return;
                    }
                    channel->setPassword(args[paramIndex]);
                    appliedModes += "+k";
                    appliedParams.push_back(args[paramIndex]);
                    paramIndex++; 
                }
                else
                {
                    channel->clearPassword();
                    appliedModes += "-k";
                }
                break;

            case 'l':
                if (adding)
                {
                    if (paramIndex >= args.size())
                    {
                        client.sendMessage("461 MODE :Not enough parameters");
                        return;
                    }
                    size_t limit = atoi(args[paramIndex].c_str());
                    if (limit <= 0)
                    {
                        client.sendMessage("461 MODE :Limit must be > 0");
                        return;
                    }
                    channel->setLimit(limit);
                    appliedModes += "+l";
                    appliedParams.push_back(args[paramIndex]);
                    paramIndex++;
                }
                else
                {
                    if (channel->isUserLimitSet() == true)
                        appliedModes += "-l";
                    channel->clearUserLimit();
                }
                break;

            case 'o':
                if (paramIndex >= args.size())
                {
                    client.sendMessage("461 MODE :Not enough parameters");
                    return;                    
                }
                std::string nick = args[paramIndex++];
                Client *target = server.getClientByNick(nick);

                if (!target || !channel->isMember(nick))
                {
                    client.sendMessage("441 " + nick + " " + channelName + " :They aren't on that channel");
                    return;
                }
                if (adding)
                    channel->addOperator(*target);
                else
                    channel->removeOperator(*target);

                appliedModes += (adding ? "+o" : "-o");
                appliedParams.push_back(nick);

                break;
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

void handleQuit(Client &client, const std::vector<std::string> &args)
{
    // braodcast to channel
    // remove Client from channel
    // disconnect Client
}

void handleJoin(Client &client, const std::vector<std::string> &args)
{
    // check ob arg groesser als 2
    // channle name muss mit # anfangen?
    // get Channel class falls nein erstelle neuen Channel als class
    // add client to channel
    // boradcast join
}

void handleTopic(Client &client, const std::vector<std::string> &args)
{
    // arg muss groeser gleich 2 sein
    //  arg[1] = channel name
    // get channel object if not available return
    // if arg == 2
    //-> braodcast channel name or no channel topic if nothing set yet
    // if arg == 3 topic setzen (in channel struct) ((setter und getter func benutzen))
}

void handleKick(Client &client, const std::vector<std::string> &args)
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

void privMsg(Client &client, const std::vector<std::string> &args)
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
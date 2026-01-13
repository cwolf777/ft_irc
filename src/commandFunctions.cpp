#include "Client.hpp"
#include "Server.hpp"
#include "commandUtils.hpp"

void Server::handleMode(Client &client, const IrcMsg &msg)
{   
    //MODE #channel <modestring> [modeparams...] bswp: MODE #channelName +it 
    //msg.get_cmd()      == "MODE"
    //msg.get_params()   == { "#test", "+i" }


    //1. genug Parameter?

    const std::vector<std::string> &params = msg.get_params();

    if (params.size() < 2)
    {
        // sendResponse(client, ":" + _serverName + " 461 MODE :Not enough parameters\r\n");
        return;
    }

    // 2. Channel existiert?

    const std::string &channelName = params[0];

    if (_channels.find(channelName) == _channels.end())
    {
        // sendResponse(client, ":" + _serverName + " 403 " + channelName + " :No such channel\r\n");
        return; // ERR_NOSUCHCHANNEL
    }
    
    Channel &channel = _channels[channelName];

    // 3. Client ist Operator?

    if (!channel.isOperator(client))
    {
        // sendResponse(client, ":" + _serverName + " 482 " + channel.getName() + " :You're not channel operator\r\n");
        return; // ERR_CHANOPRIVSNEEDED
    }

    // 4. Modestring parsen (+ / -)

    const std::string modeStr = params[1];
    bool adding = true;
    size_t paramIndex = 2;

    //TO DO: WENN MINUS KEIN PARAMS > 2?

    for (size_t i = 0; i < modeStr.size(); ++i)
    {
        char c = modeStr[i];

        if (c == '+')
        {
            adding = true;
            continue;
        }
        if (c == '-')
        {
            adding = false;
            continue;
        }
        switch (c)
        {
            case 'i':
                channel.setInviteOnly(adding);
                break;

            case 't':
                channel.setTopicProtected(adding);
                break;

            case 'k':
                if (adding)
                {
                    if (paramIndex >= params.size())
                        break;kt nur bei Broadcast
                    if(!channel.isPasswordSet())
                    {
                        channel.setPassword(params[paramIndex]);
                    }
                    else
                        //467
                    paramIndex++;kt nur bei Broadcast
                }
                else
                    channel.clearPassword();
                break;

            case 'l':
                if (adding)
                {
                    if (paramIndex >= params.size())
                        break;
                    int limit = std::atoi(params[paramIndex++].c_str());
                    if (limit < 0) limit = 0;
                    if (limit > 100) limit = 100;
                    channel.setLimit(limit);
                }
                else
                    channel.clearUserLimit();
                break;
            default:
                // sendResponse(client, ":" + _serverName + " 472 " + c + " :is unknown mode char to me\r\n");
                break;
        }
    }
    //MODE ANEDERUNGEN BROADCASTEN: IRC KONFORM: :nick!user@host MODE #chan +kl geheim 10
    std::string response = ":" + client.getPrefix()
        + " MODE " + channelName;
    for (size_t i = 1; i < params.size(); ++i)
        response += " " + params[i];

    broadcastToChannel(client, channel, response);
}

void Server::handleTopic(Client &client, const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();

    if (params.size() < 1)
    {
        //461
        return;
    }
    const std::string &channelName = params[0];

    if (_channels.find(channelName) == _channels.end())
    {
        // sendResponse(client, ":" + _serverName + " 403 " + channelName + " :No such channel\r\n");
        return; // ERR_NOSUCHCHANNEL
    }
    Channel &channel = _channels[channelName];

    if(!channel.isMember(client.getNickname()))
    {

        //442
        return;
    }

    if (params.size() == 1)
    {
        if (channel.getTopic().empty())
        {
            sendResponse(
            client,
            ":" + _serverName + " 331 " + client.getNickname() +
            " " + channel.getName() +
            " :No topic is set"
            );
        }
        else
        {
            sendResponse(
            client,
            ":" + _serverName + " 332 " + client.getNickname() +
            " " + channel.getName() +
            " :" + channel.getTopic()
            );
        }
        return; 
    }

    if (channel.isTopicProtected() && !channel.isOperator(client))
    {
        //482 not operator
        return;
    }
    std::string newTopic = params[1];
    if(!newTopic.empty() && newTopic[0] == ':')
    {
        newTopic.erase(0,1);
    }

    channel.setTopic(newTopic);

    std::string response = ":" + client.getPrefix()
    + " TOPIC " + channelName + " :" + newTopic;

    broadcastToChannel(client, channel, response);
}

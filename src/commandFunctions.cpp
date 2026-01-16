#include "Client.hpp"
#include "Server.hpp"
#include "commandUtils.hpp"

void Server::handleMode(Client &client, const IrcMsg &msg)
{
    // MODE #channel <modestring> [modeparams...] bswp: MODE #channelName +it
    // msg.get_cmd()      == "MODE"
    // msg.get_params()   == { "#test", "+i" }

    // 1. genug Parameter?

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

    // TO DO: WENN MINUS KEIN PARAMS > 2?

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
                    break;
                if (!channel.isPasswordSet())
                {
                    channel.setPassword(params[paramIndex]);
                }
                else
                    // 467
                    paramIndex++;
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
                if (limit < 0)
                    limit = 0;
                if (limit > 100)
                    limit = 100;
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
    // MODE ANEDERUNGEN BROADCASTEN: IRC KONFORM: :nick!user@host MODE #chan +kl geheim 10
    std::string response = ":" + client.getPrefix() + " MODE " + channelName;
    for (size_t i = 1; i < params.size(); ++i)
        response += " " + params[i];

    response += "\r\n";
    broadcastToChannel(client, channel, response);
    sendResponse(client, response);
}

void Server::handleTopic(Client &client, const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();

    if (params.size() < 1)
    {
        sendResponse(client, ":" + _serverName + " 461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n");
        return;
    }

    const std::string &channelName = params[0];
    if (_channels.find(channelName) == _channels.end())
    {
        sendResponse(client, ":" + _serverName + " 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
        return;
    }

    Channel &channel = _channels[channelName];
    if (!channel.isMember(client.getNickname()))
    {
        sendResponse(client, ":" + _serverName + " 442 " + client.getNickname() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }

    if (params.size() == 1)
    {
        if (channel.getTopic().empty())
        {
            sendResponse(client, ":" + _serverName + " 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n");
        }
        else
        {
            sendResponse(client, ":" + _serverName + " 332 " + client.getNickname() + " " + channelName + " :" + channel.getTopic() + "\r\n");
            // std::string setter = client.getNickname(); // Oder wer es wirklich gesetzt hat
            // std::string timestamp = "1705416000";      // Beispiel-Timestamp (oder nutze time(0))

            // std::string reply333 = ":" + _serverName + " 333 " + client.getNickname() + " " + channelName + " " + setter + " " + timestamp + "\r\n";
            // sendResponse(client, reply333);
        }
        return;
    }

    if (channel.isTopicProtected() && !channel.isOperator(client))
    {
        sendResponse(client, ":" + _serverName + " 482 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }

    std::string newTopic = params[1];

    channel.setTopic(newTopic);
    std::string response = ":" + client.getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\r\n";

    broadcastToChannel(client, channel, response);
    sendResponse(client, response);
}

// TODO: komischen abstand bei user fixen
void Server::handleNames(Client &client, const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();
    if (params.empty())
        return;

    const std::string &channelName = params[0];

    if (_channels.find(channelName) == _channels.end())
    {
        // Optional: ERR_NOSUCHCHANNEL
        // sendResponse(client, ":" + _serverName + " 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
        return;
    }

    Channel &channel = _channels[channelName];

    std::string names;
    for (auto it = channel.getMembers().begin(); it != channel.getMembers().end(); ++it)
    {
        const Client *member = *it;

        if (channel.isOperator(*member))
            names += "@";
        // else if (channel.hasVoice(*member))
        //     names += "+";

        names += member->getNickname();
        names += " ";
    }

    // 3. 353 RPL_NAMREPLY senden
    std::string reply =
        ":" + _serverName +
        " 353 " + client.getNickname() +
        " = " + channelName +
        " :" + names + "\r\n";

    sendResponse(client, reply);

    // 4. 366 RPL_ENDOFNAMES senden
    std::string end =
        ":" + _serverName +
        " 366 " + client.getNickname() +
        " " + channelName +
        " :End of /NAMES list.\r\n";

    sendResponse(client, end);
}

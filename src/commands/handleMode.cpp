#include "Server.hpp"
#include "Channel.hpp"

void handleMode(Client &client, Server &server, const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();

    if (params.size() < 2)
    {
        std::string reply = ":" + server.getServerName() + " 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        server.sendMsg(client, reply);
        return;
    }

    ServerState &state = server.getServerState();
    const std::string &channelName = params[0];

    if (channelName[0] != '#')
        return;

    Channel *currChannel = state.getChannel(channelName);
    if (!currChannel)
    {
        std::string reply = ":" + server.getServerName() + " 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
        server.sendMsg(client, reply);
        return; // ERR_NOSUCHCHANNEL
    }

    if (!currChannel->isOperator(client))
    {
        std::string reply = ":" + server.getServerName() + " 482 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n";
        server.sendMsg(client, reply);
        return; // ERR_CHANOPRIVSNEEDED
    }

    const std::string modeStr = params[1];
    bool adding = true;
    size_t paramIndex = 2;

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
            currChannel->setInviteOnly(adding);
            break;

        case 't':
            currChannel->setTopicProtected(adding);
            break;

        case 'k':
            if (adding)
            {
                if (paramIndex >= params.size())
                    break;
                if (!currChannel->isPasswordSet())
                {
                    currChannel->setPassword(params[paramIndex]);
                }
                else
                {
                    std::string reply = ":" + server.getServerName() + " 467 " +
                                        client.getNickname() + " " + channelName +
                                        " :Channel key already set\r\n";
                    server.sendMsg(client, reply);
                    paramIndex++;
                }
            }
            else
                currChannel->clearPassword();
            break;

        case 'l':
            if (adding)
            {
                if (paramIndex >= params.size())
                    break;
                int limit = std::atoi(params[paramIndex++].c_str());
                if (limit < 0)
                    limit = 0;
                currChannel->setLimit(limit);
            }
            else
                currChannel->clearUserLimit();
            break;
        case 'o':
        {
            if (paramIndex >= params.size())
                break;

            const std::string &targetNick = params[paramIndex++];
            Client *target = state.getClientByNick(targetNick);

            if (!target || !currChannel->isMember(*target))
            {
                std::string reply = ":" + server.getServerName() + " 441 " +
                                    client.getNickname() + " " + targetNick +
                                    " " + channelName + " :They aren't on that channel\r\n";
                server.sendMsg(client, reply);
                break;
            }

            if (adding)
                currChannel->addOperator(target);
            else
                currChannel->removeOperator(*target);

            break;
        }
        default:
            std::string reply = ":" + server.getServerName() + " 472 " + client.getNickname() + " " + c + "  :is unknown mode char to me\r\n";
            server.sendMsg(client, reply);
            break;
        }
    }
    std::string reply = ":" + client.getPrefix() + " MODE " + channelName;
    for (size_t i = 1; i < params.size(); ++i)
        reply += " " + params[i];
    reply += "\r\n";

    server.broadcastToChannel(client, *currChannel, reply);
    server.sendMsg(client, reply);
}
#include "ModeCommand.hpp"
#include "Server.hpp"
#include "Channel.hpp"

void ModeCommand::execute(Client &client, Server &server, const IrcMsg &msg)
{
    // MODE #channel <modestring> [modeparams...] bswp: MODE #channelName +it
    // msg.get_cmd()      == "MODE"
    // msg.get_params()   == { "#test", "+i" }

    // 1. genug Parameter?

    const std::vector<std::string> &params = msg.get_params();

    if (params.size() < 2)
    {
        std::string reply = ":" + server.getServerName() + " 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        server.sendMsg(client, reply);
        return;
    }

    // 2. Channel existiert?

    ServerState &state = server.getServerState();
    const std::string &channelName = params[0];

    Channel *currChannel = state.getChannel(channelName);
    if (!currChannel)
    {
        std::string reply = ":" + server.getServerName() + " 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
        server.sendMsg(client, reply);
        return; // ERR_NOSUCHCHANNEL
    }

    // 3. Client ist Operator?

    if (!currChannel->isOperator(client))
    {
        std::string reply = ":" + server.getServerName() + " 483 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n";
        server.sendMsg(client, reply);
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
                    // 467
                    paramIndex++;
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
                if (limit > 100) // TODO: change hardcoded value
                    limit = 100;
                currChannel->setLimit(limit);
            }
            else
                currChannel->clearUserLimit();
            break;
        default:
            std::string reply = ":" + server.getServerName() + " 472 " + client.getNickname() + " " + c + "  :is unknown mode char to me\r\n";
            server.sendMsg(client, reply);
            break;
        }
    }
    // MODE ANEDERUNGEN BROADCASTEN: IRC KONFORM: :nick!user@host MODE #chan +kl geheim 10
    std::string reply = ":" + client.getPrefix() + " MODE " + channelName;
    for (size_t i = 1; i < params.size(); ++i)
        reply += " " + params[i];
    reply += "\r\n";

    server.broadcastToChannel(client, *currChannel, reply);
    server.sendMsg(client, reply);
}
#include "Server.hpp"
#include "Channel.hpp"

static std::set<std::string> split(const std::string &s, char delimiter)
{
    std::set<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        if (!token.empty())
            tokens.insert(token);
    }
    return tokens;
}

void handlePrivMsg(Client &client, Server &server, const IrcMsg &msg)
{
    const std::vector<std::string> &params = msg.get_params();

    // 411 ERR_NORECIPIENT
    if (params.empty())
    {
        server.sendMsg(client, ":" + server.getServerName() + " 411 " + client.getNickname() + " :No recipient given (PRIVMSG)\r\n");
        return;
    }

    // 412 ERR_NOTEXTTOSEND
    if (params.size() < 2 || params[1].empty())
    {
        server.sendMsg(client, ":" + server.getServerName() + " 412 " + client.getNickname() + " :No text to send\r\n");
        return;
    }

    ServerState &state = server.getServerState();
    std::string text = params[1];
    std::set<std::string> targets = split(params[0], ',');

    for (const std::string &target : targets)
    {
        // CASE: CHANNEL
        if (target[0] == '#' || target[0] == '&')
        {
            Channel *channel = state.getChannel(target);
            if (!channel)
            {
                server.sendMsg(client, ":" + server.getServerName() + " 401 " + client.getNickname() + " " + target + " :No such nick/channel\r\n");
                continue;
            }

            // 404 ERR_CANNOTSENDTOCHAN
            if (!channel->isMember(client))
            {
                server.sendMsg(client, ":" + server.getServerName() + " 404 " + client.getNickname() + " " + target + " :Cannot send to channel\r\n");
                continue;
            }

            std::string reply = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + text + "\r\n";
            server.broadcastToChannel(client, *channel, reply);
        }
        // CASE: PRIVATE MESSAGE
        else
        {
            Client *receiver = state.getClientByNick(target);
            if (!receiver)
            {
                server.sendMsg(client, ":" + server.getServerName() + " 401 " + client.getNickname() + " " + target + " :No such nick/channel\r\n");
                continue;
            }

            std::string reply = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + text + "\r\n";
            server.sendMsg(*receiver, reply);
        }
    }
}
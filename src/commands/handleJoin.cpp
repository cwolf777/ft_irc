#include "Server.hpp"
#include "Channel.hpp"
#include <algorithm>

static bool isValidChannelName(const std::string &name)
{
    if (name.empty() || name.size() > 50)
        return false;

    if (name[0] != '#' || name[0] != '&')
        return false;
    for (size_t i = 0; i < name.length(); ++i)
    {
        if (name[i] == ' ' || name[i] == ',' || name[i] == '\a')
            return false;
    }
    return true;
}

static std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        if (!token.empty())
            tokens.push_back(token);
    }
    return tokens;
}

static void processJoin(Client &client, Server &server, const std::string &name, const std::string &key)
{
    ServerState &state = server.getServerState();

    if (!isValidChannelName(name))
    {
        server.sendMsg(client, ":" + server.getServerName() + " 476 " + client.getNickname() + " " + name + " :Bad Channel Mask\r\n");
        return;
    }

    Channel *channel = state.getChannel(name);
    if (!channel)
    {
        Channel newChannel(name);
        newChannel.addOperator(&client);
        state.addChannel(newChannel);
        channel = state.getChannel(name);
    }

    if (channel->isInviteOnly() && !channel->isInvited(client))
    {
        server.sendMsg(client, ":" + server.getServerName() + " 473 " + client.getNickname() + " " + name + " :Cannot join channel (+i)\r\n");
        return;
    }
    if (client.getChannels().size() >= state.getChannelLimit())
    {
        server.sendMsg(client, ":" + server.getServerName() + " 405 " + client.getNickname() + " " + name + " :You have joined too many channels\r\n");
        return;
    }
    if (channel->isUserLimitSet() && channel->getMembers().size() >= channel->getUserLimit())
    {
        server.sendMsg(client, ":" + server.getServerName() + " 471 " + client.getNickname() + " " + name + " :Cannot join channel (+l)\r\n");
        return;
    }
    if (channel->isPasswordSet() && channel->getPassword() != key)
    {
        server.sendMsg(client, ":" + server.getServerName() + " 475 " + client.getNickname() + " " + name + " :Cannot join channel (+k)\r\n");
        return;
    }

    // add client
    channel->addMember(&client);
    client.joinChannel(channel);

    std::cout << *channel << std::endl;
    // send to everyone in channel a message
    std::string joinMsg = ":" + client.getPrefix() + " JOIN :" + name + "\r\n";
    server.broadcastToChannel(client, *channel, joinMsg);

    // send client the topic of the channel he joined
    IrcMsg topic("TOPIC " + name + "\r\n");
    server.handleRequest(client, topic);
    IrcMsg names("NAMES " + name + "\r\n");
    server.handleRequest(client, names);
}

void handleJoin(Client &client, Server &server, const IrcMsg &msg)
{
    // ERR_NEEDMOREPARAMS Join command needs channel name as parameter : JOIN #foobar
    if (msg.get_params().empty())
    {
        server.sendMsg(client, ":" + server.getServerName() + " 461 " + client.getNickname() + "JOIN :Not enough parameters\r\n");
        return;
    }

    // splitting the channels and keys from params
    std::vector<std::string> channels = split(msg.get_params()[0], ',');
    std::vector<std::string> keys;
    if (msg.get_params().size() > 1)
        keys = split(msg.get_params()[1], ',');

    // loop through channels to join each channel
    for (size_t i = 0; i < channels.size(); i++)
    {
        std::string name = channels[i];
        std::string key = (i < keys.size()) ? keys[i] : "";
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        processJoin(client, server, name, key);
    }
}
#include "JoinCommand.hpp"
#include "Server.hpp"
#include "Channel.hpp"

// TODO: cleaner machen
void JoinCommand::execute(Client &client, Server &server, const IrcMsg &msg)
{
    // ERR_NEEDMOREPARAMS Join command needs channel name as parameter : JOIN #foobar
    if (msg.get_params().empty())
    {
        std::string reply = ":" + server.getServerName() + " 461 " + client.getNickname() + "JOIN :Not enough parameters\r\n";
        server.sendMsg(client, reply);
        return;
    }

    ServerState &state = server.getServerState();
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
    for (size_t i = 0; i < joinList.size(); i++)
    {
        std::string currChannelName = joinList[i];
        std::string currPass = (i < passwordList.size()) ? passwordList[i] : "";

        // 403 ERR_NOSUCHCHANNEL Channel currChannelName must start with # or &
        if (currChannelName.empty() || (currChannelName[0] != '&' && currChannelName[0] != '#'))
        {
            std::string reply = ":" + server.getServerName() + " 403 " + client.getNickname() + " " + currChannelName + " :No such channel\r\n";
            server.sendMsg(client, reply);
            continue;
        }

        //  if channel dont exist => create channel
        Channel *currChannel = state.getChannel(currChannelName);
        if (!currChannel)
        {
            Channel newChannel(currChannelName);
            newChannel.addOperator(&client); // first user becomes admin
            state.addChannel(newChannel);
            currChannel = state.getChannel(currChannelName);
        }

        // 473 ERR_INVITEONLYCHAN client has to be in invited vector
        if (currChannel->isInviteOnly() && !currChannel->isInvited(client))
        {
            server.sendMsg(client, ":" + server.getServerName() + " 473 " + client.getNickname() + " " + currChannelName + " ::Cannot join channel (+i)\r\n");
            return;
        }
        // 405 ERR_TOOMANYCHANNELS  check server Channellimit
        if (client.getChannels().size() >= state.getChannelLimit())
        {
            server.sendMsg(client, ":" + server.getServerName() + " 405 " + client.getNickname() + " " + currChannelName + " :You have joined too many channels\r\n");
            return;
        }
        // 471 ERR_CHANNELISFULL check if channel is full
        if (currChannel->isUserLimitSet() && currChannel->getMembers().size() >= currChannel->getUserLimit())
        {
            std::string reply = ":" + server.getServerName() + " 471 " + client.getNickname() + " " + currChannelName + " :Cannot join channel (+l)\r\n";
            server.sendMsg(client, reply);
            continue;
        }
        // 475 ERR_BADCHANNELKEY check if password is set and correct
        if (currChannel->isPasswordSet() && currChannel->getPassword() != currPass)
        {
            std::string reply = ":" + server.getServerName() + " 475 " + client.getNickname() + " " + currChannelName + " :Cannot join channel (+k)\r\n";
            server.sendMsg(client, reply);
            continue;
        }
        // add client
        currChannel->addMember(&client);
        client.joinChannel(currChannel);

        std::cout << *currChannel << std::endl;
        // send to everyone in channel a message
        std::string joinMsg = ":" + client.getPrefix() + " JOIN :" + currChannelName + "\r\n";
        server.broadcastToChannel(client, *currChannel, joinMsg);

        // send client the topic of the channel he joined
        IrcMsg topic("TOPIC " + currChannelName + "\r\n");
        server.handleRequest(client, topic);
        IrcMsg names("NAMES " + currChannelName + "\r\n");
        server.handleRequest(client, names);
    }
}
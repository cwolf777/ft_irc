// #include "Client.hpp"
// #include "Server.hpp"
// #include "commandUtils.hpp"

void Server::handleTopic(Client &client, const IrcMsg &msg)
{
    // arg muss groeser gleich 2 sein
    //  arg[1] = channel name
    // get channel object if not available return
    // if arg == 2
    //-> braodcast channel name or no channel topic if nothing set yet
    // if arg == 3 topic setzen (in channel struct) ((setter und getter func benutzen))

    if ( msg.get_params().size() < 1)
    {
        client.sendMessage("461 TOPIC :Not enough parameters");
        return;
    }
    std::string channelName = msg.get_params()[0];

    Channel *channel = nullptr;
    try
    {
        channel = &getChannel(channelName);
    } catch (...) 
    {
        client.sendMessage("403 " + channelName + " :No such channel");
        return;
    }

    if (!channel->isMember(client.getNickname()))
    {
        client.sendMessage("442 " + channelName + " :You're not on that channel");
        return;        
    }

    if (msg.get_params().size() == 1)
    {
        std::string topic = channel->getTopic();
        if (topic.empty())
        {
            client.sendMessage("331 " + channelName + " :No topic is set");
        }
        else
        {
            client.sendMessage("332 " + channelName + " :" + topic);
        }
        return;
    }
    if (channel->isTopicProtected() && !channel->isOperator(client))
    {
        client.sendMessage("482 " + channelName + " :You're not channel operator");
        return; 
    }
    std::string newTopic = msg.get_params()[1];
    channel->setTopic(newTopic);

    //TOPIC AN ALLE CHANNEL MITGLIEDER SENDEN 
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

    //KICK <channel> <user> [:<comment>]

    if(msg.get_params().size() < 2)
    {
        client.sendMessage("461 KICK :Not enough parameters");
        return;   
    }

    std::string channelName = msg.get_params()[0];
    std::string targetNick = msg.get_params()[1];
    std::string reason = (msg.get_params().size() > 2) ? msg.get_params()[2] : "No reason";

    Channel *channel = nullptr;

    try
    {
        channel = &getChannel(channelName);
    } catch (...)
    {
        client.sendMessage("403 " + channelName + " :No such channel");
        return;
    }

    if (!channel->isMember(client.getNickname()))
    {
        client.sendMessage("442 " + channelName + " :You're not on that channel");
        return;
    }
    if (!channel->isOperator(client))
    {
        client.sendMessage("482 " + channelName + " :You're not channel operator");
        return;
    }
    Client &target = getClientByNick(targetNick);
    if(!channel->isMember(target.getNickname()))
    {
        client.sendMessage("441 " + targetNick + " " + channelName + " :They aren't on that channel");
        return;
    }
    channel->removeMember(target);
    channel->removeOperator(target);

    //BROADCAST AN ALLE CHANNEL MEMBER

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
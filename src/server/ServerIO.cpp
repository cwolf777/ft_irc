#include "Server.hpp"

void Server::sendMsg(const Client &client, const std::string &msg) const
{
    std::cout << "server to client [" << client.getFd() << "]{" << client.getUsername() << "} : " << msg << std::endl;
    send(client.getFd(), msg.c_str(), msg.size(), 0);
}

void Server::sendMsg(const Client &client, const char *msg) const
{
    std::cout << "server to client [" << client.getFd() << "]{" << client.getUsername() << "} : " << msg << std::endl;

    send(client.getFd(), msg, std::strlen(msg), 0);
}

void Server::sendWelcomeMessage(const Client &client) const
{
    std::string msg(":" + _serverName + " 001 " + client.getNickname() + " :Welcome to the IRC Network " + client.getPrefix() + "\r\n");
    sendMsg(client, msg);
}

void Server::broadcastToChannel(const Client &client, const Channel &channel, const std::string &msg)
{
    const std::string senderNick = client.getNickname();

    for (const Client *currentClient : channel.getMembers())
    {
        if (currentClient->getNickname() == senderNick)
            continue;

        sendMsg(*currentClient, msg);
    }
}

bool Server::receive(int fd, std::string &data)
{
    char buffer[512];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes == 0)
        return false;

    if (bytes < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return true;
        std::cerr << "Recv error on fd " << fd << ": " << std::strerror(errno) << std::endl;
        return false;
    }
    data.assign(buffer, bytes);
    return true;
}
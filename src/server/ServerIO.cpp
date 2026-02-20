#include "Server.hpp"

void Server::sendMsg(Client &client, const std::string &msg)
{
    if (msg.empty() || msg.size() > sizeof(char) * 512)
        return;
    if (client.getFd() < 0)
        return;

    client.appendToWriteBuffer(msg);

    std::string &output = client.getWriteBuffer();
    if (output.empty())
        return;

    std::cout << Color::BLUE << "server to  [" << client.getFd() << "]{" << client.getNickname() << "} : " << output << Color::RESET << std::endl;
    ssize_t bytesSent = send(client.getFd(), output.data(), output.size(), 0);
    if (bytesSent < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            pollfd &pfd = getPollByFd(client.getFd());
            pfd.events |= POLLOUT; // enable POLLOUT to know when we can send again
            return;
        }
        throw ServerException("Failed to send message to client " + client.getNickname() + " (fd: " + std::to_string(client.getFd()) + "): " + std::strerror(errno));
    }
    output.erase(0, bytesSent);

    pollfd &pfd = getPollByFd(client.getFd());
    if (!client.getWriteBuffer().empty())
        pfd.events |= POLLOUT; // partial write -> keep flushing later
    else
        pfd.events &= ~POLLOUT;
}

void Server::sendWelcomeMessage(Client &client)
{
    std::string nick = client.getNickname();
    std::string user = client.getUsername();
    std::string host = client.getHostname();

    // 001: Welcome
    sendMsg(client, ":" + _serverName + " 001 " + nick + " :Welcome to the IRC Network " + nick + "!" + user + "@" + host + "\r\n");

    // 002: Your Host
    sendMsg(client, ":" + _serverName + " 002 " + nick + " :Your host is " + _serverName + ", running version 1.0\r\n");

    // 003: Created
    sendMsg(client, ":" + _serverName + " 003 " + nick + " :This server was created " + _creationDate + "\r\n");

    // 004: MyInfo (Servername, Version, UserModes, ChannelModes)
    sendMsg(client, ":" + _serverName + " 004 " + nick + " " + _serverName + " 1.0 o i,k,l,t,o\r\n");

    // 005: ISUPPORT (Features supported by the server)
    sendMsg(client, ":" + _serverName + " 005 " + nick + " CHANMODES=,k,l,it PREFIX=(o)@ :are supported by this server\r\n");
}

void Server::broadcastToChannel(const Client &client, const Channel &channel, const std::string &msg)
{
    const std::string senderNick = client.getNickname();

    for (Client *currentClient : channel.getMembers())
    {
        if (currentClient->getNickname() == senderNick)
            continue;

        sendMsg(*currentClient, msg);
    }
}

bool Server::receive(int fd, std::string &data)
{
    char buffer[4096];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes == 0)
        return false;

    if (bytes < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return true;
        throw ServerException("Failed to receive data from client (fd: " + std::to_string(fd) + "): " + std::strerror(errno));
    }
    data.assign(buffer, bytes);
    return true;
}

void Server::flushClient(Client &client)
{
    static const size_t MAX_FLUSH_BYTES = 64 * 1024; // 64KB pro poll-tick
    size_t sentTotal = 0;
    std::string &output = client.getWriteBuffer();
    while (!output.empty() && sentTotal < MAX_FLUSH_BYTES)
    {
        size_t toSend = std::min(output.size(), MAX_FLUSH_BYTES - sentTotal);
        ssize_t bytesSent = send(client.getFd(), output.data(), toSend, 0);
        if (bytesSent < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return; // keep POLLOUT enabled
            throw ServerException("Failed to flush data to client " + client.getNickname() + " (fd: " + std::to_string(client.getFd()) + "): " + std::strerror(errno));
        }
        if (bytesSent == 0)
            break;
        sentTotal += bytesSent;
        output.erase(0, bytesSent);
    }
    pollfd &pfd = getPollByFd(client.getFd());
    if (output.empty())
        pfd.events &= ~POLLOUT; // // disable POLLOUT
    else
        pfd.events |= POLLOUT;
}

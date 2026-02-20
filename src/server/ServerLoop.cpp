#include "Server.hpp"

extern volatile sig_atomic_t server_running;

void Server::connectClient(void)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(_polls[0].fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
        throw ServerException("Error client accept");

    fcntl(client_fd, F_SETFL, O_NONBLOCK); // sets socket to be non blocking
    char hostname[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, hostname, INET_ADDRSTRLEN);

    // add client_fd to _polls_fds and create Client with same fd and add to _clients
    _polls.push_back(pollfd{client_fd, POLLIN, 0});
    _state._clients.insert({client_fd, Client(client_fd, hostname)});
    std::cout << Color::MAGENTA << "new client connected: " << hostname << " (FD: " << client_fd << ")" << Color::RESET << std::endl;
}

void Server::disconnectClient(Client &client)
{
    int client_fd = client.getFd();
    std::cout << Color::MAGENTA << "Disconnected: " << client.getNickname() << " (FD: " << client_fd << ")" << Color::RESET << std::endl;

    auto poll_it = std::find_if(_polls.begin(), _polls.end(),
                                [client_fd](pollfd p)
                                { return p.fd == client_fd; });

    if (poll_it != _polls.end())
        _polls.erase(poll_it);

    for (Channel *chan : client.getChannels())
    {
        chan->removeMember(client);
        chan->removeOperator(client);
        if (chan->getMembers().empty())
            _state._channels.erase(chan->getName());
    }
    if (_state._clients.erase(client_fd) == 0)
        std::cerr << Color::RED << "Warning: FD " << client_fd << " was already erased from Map." << Color::RESET << std::endl;
    else
    {
        std::cout << Color::MAGENTA << "(FD: " << client_fd << ") closed" << Color::RESET << std::endl;
        close(client_fd); // important
    }
}

void Server::run()
{
    _polls.push_back({_server_fd, POLLIN, 0});
    while (server_running)
    {
        // setting poll for clients
        int ret = poll(_polls.data(), _polls.size(), 1000); // after 1 sec wake up

        if (ret < 0)
        {
            if (!server_running)
                break; // signal catched
            throw ServerException("Poll failed");
        }

        if (ret == 0) // poll timeout expired
            continue;

        if (_polls[0].revents & (POLLIN | POLLERR))
        {
            if (_polls[0].revents & POLLIN)
                connectClient();
            else
                throw ServerException("Listening socket error");
        }

        for (int i = (int)_polls.size() - 1; i >= 1; --i)
        {
            if (!server_running)
                break;
            if (_polls[i].revents & (POLLIN | POLLHUP | POLLERR | POLLNVAL | POLLOUT))
            {
                Client *client = _state.getClientByFd(_polls[i].fd);
                if (!client || (_polls[i].revents & POLLNVAL))
                {
                    std::cerr << Color::RED << "Invalid or missing client for fd: " << _polls[i].fd << Color::RESET << std::endl;
                    _polls.erase(_polls.begin() + i);
                    continue;
                }

                if (_polls[i].revents & POLLOUT)
                    flushClient(*client);
                std::string rawData;
                if ((_polls[i].revents & (POLLERR | POLLHUP)) && !(_polls[i].revents & POLLIN))
                {
                    disconnectClient(*client);
                    continue;
                }
                try
                {
                    if (!receive(_polls[i].fd, rawData))
                    {
                        disconnectClient(*client);
                        continue;
                    }
                    processData(*client, rawData);
                    if (client->isDead())
                    {
                        disconnectClient(*client);
                        continue;
                    }
                }
                catch (const ServerException &e)
                {
                    std::cerr << Color::RED << e.what() << Color::RESET << std::endl;
                    disconnectClient(*client);
                }
                catch (const std::overflow_error &e)
                {
                    std::cerr << Color::RED << e.what() << Color::RESET << '\n';
                    disconnectClient(*client);
                }
                catch (const IrcMsg::IrcMsgException &e)
                {
                    std::cerr << Color::YELLOW << e.what() << Color::RESET << std::endl;
                }
            }
        }
    }
    // std::cout << "\nserver closed..." << std::endl;
}

void Server::processData(Client &client, std::string &rawData)
{

    client.appendToReadBuffer(rawData);
    std::string &buffer = client.getReadBuffer();
    size_t pos;
    while ((pos = buffer.find("\r\n")) != std::string::npos)
    {
        std::string line = buffer.substr(0, pos + 2);
        buffer.erase(0, pos + 2);
        IrcMsg msg;
        msg.create(line);
        std::cout << Color::GREEN << "[" << client.getFd() << "]{" << client.getNickname() << "} : " << line << Color::RESET << "\n";
        handleRequest(client, msg);
        if (client.isDead())
            return;
    }
}

void Server::shutdown(const std::string &reason)
{
    std::cout << Color::MAGENTA << "\nserver closed..." << Color::RESET << std::endl;
    while (!_state._clients.empty())
    {
        Client &client = _state._clients.begin()->second;
        const std::string reply = "ERROR :Closing Link: " + client.getNickname() + " (" + reason + ")\r\n";
        sendMsg(client, reply);
        disconnectClient(client);
    }
    if (_server_fd > 0)
        close(_server_fd);
}
void Server::handleRequest(Client &client, const IrcMsg &msg)
{
    _commands.dispatch(client, *this, msg);
}

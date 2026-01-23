#include "Server.hpp"

extern volatile sig_atomic_t server_running;

void Server::connectClient(void)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(_polls[0].fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
        throw ServerException("Error client accept");

    char hostname[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, hostname, INET_ADDRSTRLEN);

    // add client_fd to _polls_fds and create Client with same fd and add to _clients
    _polls.push_back(pollfd{client_fd, POLLIN, 0});
    _state._clients.insert({client_fd, Client(client_fd, hostname)});
    std::cout << "new client connected: " << hostname << " (FD: " << client_fd << ")" << std::endl;
}

// TODO: cleaner machen
void Server::disconnectClient(Client &client)
{
    int client_fd = client.getFd();
    std::cout << "Disconnected: " << client.getNickname() << " (FD: " << client_fd << ")" << std::endl;
    std::cout << client << std::endl;

    auto poll_it = std::find_if(_polls.begin(), _polls.end(),
                                [client_fd](pollfd p)
                                { return p.fd == client_fd; });

    if (poll_it != _polls.end())
        _polls.erase(poll_it);

    if (_state._clients.erase(client_fd) == 0)
        std::cerr << "Warning: FD " << client_fd << " was already gone from Map." << std::endl;
    else
        close(client_fd); // important
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

        if (_polls[0].revents & POLLIN) // TODO: check for more poll flags
            connectClient();

        for (size_t i = _polls.size() - 1; i >= 1; i--)
        {
            if (!server_running)
                break;
            if (_polls[i].revents & POLLIN)
            {
                Client *client = _state.getClientByFd(_polls[i].fd);
                if (!client)
                {
                    std::cerr << "Client with fd: " << _polls[i].fd << " does not exist!" << std::endl;
                    _polls.erase(_polls.begin() + i);
                    continue;
                }
                std::string rawData;
                try
                {
                    if (!receive(_polls[i].fd, rawData))
                    {
                        disconnectClient(*client);
                        continue;
                    }
                    processBuffer(*client, rawData);
                }
                catch (const ServerException &e)
                {
                    std::cerr << e.what() << std::endl;
                    disconnectClient(*client);
                }
                catch (const IrcMsg::IrcMsgException &e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
        }
    }
    shutdown("Server shutting down");
    std::cout << "\nserver closed..." << std::endl;
}

void Server::processBuffer(Client &client, std::string &rawData)
{
    client.getBuffer() += rawData;
    std::string &buffer = client.getBuffer();
    size_t pos;
    while ((pos = buffer.find("\r\n")) != std::string::npos)
    {
        std::string line = buffer.substr(0, pos + 2);
        buffer.erase(0, pos + 2);

        if (line.length() > 512)
        {
            disconnectClient(client);
            return;
        }
        IrcMsg msg;
        msg.create(line);
        std::cout << "[" << client.getFd() << "]{" << client.getUsername() << "} : " << line << "\n";
        handleRequest(client, msg);
    }
}

void Server::shutdown(const std::string &reason)
{
    while (!_state._clients.empty())
    {
        Client &client = _state._clients.begin()->second;
        const std::string reply = "ERROR :Closing Link: " + client.getNickname() + " (" + reason + ")\r\n";
        sendMsg(client, reply);
        disconnectClient(client);
    }
}
void Server::handleRequest(Client &client, const IrcMsg &msg)
{
    _commands.dispatch(client, *this, msg);
}

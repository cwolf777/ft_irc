
#include "Server.hpp"

Server::Server(const std::string &severName, int port, const std::string &password) : _serverName(severName),
                                                                                      _serverPrefix(":" + severName + " "),
                                                                                      _port(port),
                                                                                      _password(password),
                                                                                      _server_fd(-1)
{
    std::fill(_poll_fds.begin(), _poll_fds.end(), pollfd{});
}

Server::Server(const Server &other) : _port(other._port),
                                      _password(other._password),
                                      _server_fd(other._server_fd),
                                      _clients(other._clients),
                                      _channels(other._channels),
                                      _poll_fds(other._poll_fds)
{
}

Server &Server::operator=(const Server &other)
{
    if (this == &other)
        return *this;
    _serverName = other._serverName;
    _serverPrefix = other._serverPrefix;
    _port = other._port;
    _password = other._password;
    _server_fd = other._server_fd;
    _poll_fds = other._poll_fds;
    _clients = other._clients;
    _channels = other._channels;
    return *this;
}

Server::~Server()
{
    if (_server_fd > 0)
        close(_server_fd);
    // TODO: Where do i close the fd from client
    //  for (Client c : _clients)
    //  {
    //      if ( > 0)
    //          close(client.fd);
    //  }
}

void Server::init(int domain)
{
    // Creating the server socket
    // AF_INET (Ipv4), SOCK_STREAM (TCP), 0 (Default) => TCP
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (_server_fd < 0)
        throw ServerException("Failed to create socket");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(domain); // INADDR_LOOPBACK (localhost)
    addr.sin_port = htons(_port);         // Port 6667

    // Binds the _server_fd to the given address (localhost) and port (6667)
    // The port is now reserved
    if (bind(_server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        throw ServerException("Failed to bind socket on port " + std::to_string(_port));
    if (listen(_server_fd, 5) < 0)
        throw ServerException("Failed ");

    // Changing _server_fd with Flags to be non blocking i/o
    int flags = fcntl(_server_fd, F_GETFL, 0);
    fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK);
    std::cout << "server runnning on port " << _port << "...\n";
}

void Server::run()
{
    _poll_fds.push_back({_server_fd, POLLIN, 0});
    while (true)
    {
        // setting poll for clients
        poll(_poll_fds.data(), _poll_fds.size(), -1); // -1 blocking

        if (_poll_fds[0].revents & POLLIN)
        {
            connectClient();
        }
        for (size_t i = _poll_fds.size() - 1; i >= 1; i--)
        {
            if (_poll_fds[i].revents & POLLIN)
            {
                char buffer[512];
                ssize_t bytes_recvd;
                Client &currClient = _clients[_poll_fds[i].fd];
                bytes_recvd = recv(currClient.getFd(), buffer, sizeof(buffer) - 1, 0);
                if (bytes_recvd <= 0)
                {
                    disconnectClient(currClient);
                    continue;
                }
                if (bytes_recvd >= 512)
                {
                    std::cout << "SPAM!!! Client disconnected!\n";
                    disconnectClient(currClient);
                    continue;
                }
                buffer[bytes_recvd] = '\0';
                std::string msg(buffer);
                try
                {
                    size_t pos = 0;
                    while ((pos = msg.find("\r\n")) != std::string::npos)
                    {

                        std::cout << "client to server [" << currClient.getFd() << "]{" << currClient.getUsername() << "} : " << msg << "\n";
                        IrcMsg request;
                        request.create(msg.substr(0, pos + 2));
                        handleRequest(currClient, request);
                        msg = msg.substr(pos + 2);
                    }
                }
                catch (const IrcMsg::IrcMsgException &e)
                {
                    std::cerr << e.what() << currClient << std::endl;
                }
                catch (const ServerException &e)
                {
                    std::cerr << e.what() << std::endl;
                    disconnectClient(currClient);
                }
            }
        }
    }
    close(_server_fd);
}

void Server::sendResponse(const Client &client, const IrcMsg &response) const
{
    std::cout << "server to client [" << client.getFd() << "]{" << client.getUsername() << "} : " << response.get_msg() << std::endl;
    send(client.getFd(), response.get_msg().c_str(), response.get_msg().size(), 0);
}

void Server::sendResponse(const Client &client, const std::string &msg) const
{
    std::cout << "server to client [" << client.getFd() << "]{" << client.getUsername() << "} : " << msg << std::endl;
    send(client.getFd(), msg.c_str(), msg.size(), 0);
}

void Server::sendResponse(const Client &client, const char *msg) const
{
    std::cout << "server to client [" << client.getFd() << "]{" << client.getUsername() << "} : " << msg << std::endl;

    send(client.getFd(), msg, std::strlen(msg), 0);
}

void Server::sendWelcomeMessage(const Client &client) const
{
    std::string msg(":" + _serverPrefix + "001 " + client.getNickname() + " :Welcome to the IRC Network " + client.getPrefix() + "\r\n");
    sendResponse(client, msg);
    // sendResponse(client, _serverPrefix + "002 " + client.getNickname() + " :Your host is " + _serverName + ", running version 1.0\r\n");
}

void Server::broadcastToChannel(const Client &client, Channel &channel, const std::string &msg)
{
    const std::string senderNick = client.getNickname();

    for (const Client *currentClient : channel.getMembers())
    {
        // Sende NICHT an den ursprünglichen Sender
        if (currentClient->getNickname() == senderNick)
            continue;

        sendResponse(*currentClient, msg);
    }
}

void Server::connectClient(void)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(_poll_fds[0].fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
        throw ServerException("Error client accept");

    char hostname[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, hostname, INET_ADDRSTRLEN);

    // add client_fd to _polls_fds and create Client with same fd and add to _clients
    _poll_fds.push_back(pollfd{client_fd, POLLIN, 0});
    _clients[client_fd] = Client(client_fd, hostname);
    std::cout << "new client connected: " << hostname << " (FD: " << client_fd << ")" << std::endl;
}

void Server::disconnectClient(Client &client)
{
    int client_fd = client.getFd();

    auto poll_it = std::find_if(_poll_fds.begin(), _poll_fds.end(),
                                [client_fd](pollfd p)
                                {
                                    return p.fd == client_fd;
                                });

    if (poll_it == _poll_fds.end())
    {
        throw ServerException("Error: Client not found in the _poll_fds list.");
        std::cerr << "Error: Client not found in the _poll_fds list." << std::endl;
    }
    if (_clients.erase(client_fd) == 0)
    {
        throw ServerException("Error: Client not found in the _clients list.");
        std::cerr << "Error: Client not found in the _clients list." << std::endl;
    }

    std::cout << "Disconnected!" << std::endl;
    std::cout << client << std::endl;
    // TODO: close client_fd here or in Client destructor ??
    _poll_fds.erase(poll_it);
}
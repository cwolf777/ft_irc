
#include "Server.hpp"

Server::Server(int port, const std::string &password) : _port(port), _password(password), _server_fd(-1)
{
    std::fill(_poll_fds.begin(), _poll_fds.end(), pollfd{});
}

Server::Server(const Server &other) : _port(other._port),
                                      _password(other._password),
                                      _server_fd(other._server_fd),
                                      _clients(other._clients),
                                      _poll_fds(other._poll_fds)
{
}

Server &Server::operator=(const Server &other)
{
    if (this == &other)
        return *this;
    _port = other._port;
    _password = other._password;
    _server_fd = other._server_fd;
    _poll_fds = other._poll_fds;
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
                bytes_recvd = recv(_poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes_recvd <= 0)
                {
                    disconnectClient(_clients[i - 1]);
                    continue;
                }
                if (bytes_recvd >= 512)
                {
                    std::cout << "SPAM!!! Client disconnected!\n";
                    disconnectClient(_clients[i - 1]);
                    continue;
                }
                buffer[bytes_recvd] = '\0';
                std::string msg(buffer);
                try
                {
                    size_t pos = 0;
                    while ((pos = msg.find("\r\n")) != std::string::npos)
                    {

                        std::cout << "Client: " << msg << "\n";
                        IrcMsg request;
                        request.create(msg.substr(0, pos + 2));
                        std::cout << request << std::endl;
                        handleRequest(_clients[i - 1], request);
                        msg = msg.substr(pos + 2);
                    }
                }
                catch (const IrcMsg::IrcMsgException &e)
                {
                    std::cerr << e.what() << std::endl;
                }
                catch (const ServerException &e)
                {
                    std::cerr << e.what() << std::endl;
                    disconnectClient(_clients[i - 1]);
                }
            }
        }
    }
    close(_server_fd);
}

void Server::sendResponse(const Client &client, const IrcMsg &response) const
{
    std::cout << "Server to client[" << client.getFd() << "]: " << response.get_msg() << std::endl;
    send(client.getFd(), response.get_msg().c_str(), response.get_msg().size(), 0);
}

void Server::sendResponse(const Client &client, const std::string &msg) const
{
    std::cout << "Server to client[" << client.getFd() << "]: " << msg << std::endl;
    send(client.getFd(), msg.c_str(), msg.size(), 0);
}

void Server::sendResponse(const Client &client, const char *msg) const
{
    std::cout << "Server to client[" << client.getFd() << "]: " << msg << std::endl;

    send(client.getFd(), msg, std::strlen(msg), 0);
}

void Server::sendWelcomeMessage(const Client &client) const
{
    std::string msg(":TEST_SERVER_34 001 " + client.getNickname() + " Welcome to the IRC Network 34!");
    std::cout << "Server to client[" << client.getFd() << "]: " << msg << std::endl;
    send(client.getFd(), msg.c_str(), msg.size(), 0);
}

void Server::broadcastToChannel(const Client &client, Channel &channel, const std::string &msg)
{
    const std::string senderNick = client.getNickname();

    for (const Client &currentClient : channel.getMembers())
    {
        // Sende NICHT an den ursprünglichen Sender
        if (currentClient.getNickname() == senderNick)
            continue;

        sendResponse(currentClient, msg);
    }
}

void Server::connectClient(void)
{
    int client_fd = accept(_poll_fds[0].fd, nullptr, nullptr);
    if (client_fd >= 0)
    {
        std::cout << "new client connected!\n";
        _poll_fds.push_back(pollfd{client_fd, POLLIN, 0});
        _clients.push_back(Client(client_fd));
    }
    // TODO: handle client_fd < 0
}

void Server::disconnectClient(Client &client)
{
    int client_fd = client.getFd();

    auto client_it = std::find_if(_clients.begin(), _clients.end(),
                                  [client_fd](const Client &c)
                                  {
                                      return c.getFd() == client_fd;
                                  });

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
    if (client_it == _clients.end())
    {
        throw ServerException("Error: Client not found in the _clients list.");
        std::cerr << "Error: Client not found in the _clients list." << std::endl;
    }

    std::cout << "Disconnected!" << std::endl;
    std::cout << client << std::endl;

    _poll_fds.erase(poll_it);
    _clients.erase(client_it);
}
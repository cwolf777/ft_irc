
#include "Server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password), _server_fd(-1) {}

Server::Server(const Server &other) : _port(other._port), _password(other._password), _server_fd(other._server_fd), _clients(other._clients) {}

Server &Server::operator=(const Server &other)
{
    if (this == &other)
        return *this;
    _port = other._port;
    _password = other._password;
    _server_fd = other._server_fd;

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
    _client_fds.push_back({_server_fd, POLLIN, 0});
    while (true)
    {
        // setting poll for clients
        poll(_client_fds.data(), _client_fds.size(), -1); // -1 blocking

        if (_client_fds[0].revents & POLLIN)
        {
            int client_fd = accept(_client_fds[0].fd, nullptr, nullptr);
            if (client_fd >= 0)
            {
                std::cout << "new client connected!\n";
                _client_fds.push_back({client_fd, POLLIN, 0});
                _clients.push_back(Client(client_fd, "", false, false));
            }
        }
        for (size_t i = 1; i < _client_fds.size(); i++)
        {
            if (_client_fds[i].revents & POLLIN)
            {
                char buffer[1024];
                int n = recv(_client_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                if (n <= 0)
                {
                    std::cout << "Client disconnected!\n";
                    close(_client_fds[i].fd);
                    _client_fds.erase(_client_fds.begin() + i);
                    _clients.erase(_clients.begin() + i);
                    i--;
                    continue;
                }
                buffer[n] = '\0';
                std::string msg(buffer);
                std::cout << "Client: " << msg << "\n";
                if (msg == "CAP LS\r\n")
                {
                    send(_client_fds[i].fd, ":irc.34 CAP * LS :\r\n", 23, 0); // no CAP features
                    // send(_client_fds[i].fd, ":irc.34 001 nickname :Welcome to the irc chat, nickname!\r\n", 58, 0);
                }
                if (msg == "PASS abcdef")
                {
                    send(_client_fds[i].fd, "HALLO!\n", 8, 0);
                }
                if (!_clients[i].get_registered())
                {
                    std::string msg(":irc.34 NOTICE * :Password required. Use: PASS <password>\r\n");
                    send(_client_fds[i].fd, ":irc.34 NOTICE * :Password required. Use: PASS <password>\r\n", msg.size(), 0);
                }
            }
        }
    }
    close(_server_fd);
}
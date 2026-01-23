
#include "Server.hpp"

Server::Server(const std::string &severName, int port, const std::string &password)
    : _serverName(severName),
      _port(port),
      _password(password),
      _server_fd(-1)
{
    std::fill(_polls.begin(), _polls.end(), pollfd{});
}

Server::Server(const Server &other)
    : _serverName(other._serverName),
      _port(other._port),
      _password(other._password),
      _server_fd(other._server_fd),
      _polls(other._polls)
{
}

Server &Server::operator=(const Server &other)
{
    if (this == &other)
        return *this;
    _serverName = other._serverName;
    _port = other._port;
    _password = other._password;
    _server_fd = other._server_fd;
    _polls = other._polls;
    return *this;
}

Server::~Server()
{
    std::cout << "\nserver closed..." << std::endl;
    shutdown("Server shutting down");
    if (_server_fd > 0)
        close(_server_fd);
}

std::string Server::getServerName() const
{
    return _serverName;
}

std::string Server::getPassword() const
{
    return _password;
}

ServerState &Server::getServerState()
{
    return _state;
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


#include "Server.hpp"
#include <ctime>

static std::string getCurrentDate()
{
    std::time_t now = std::time(nullptr);
    std::tm *ltm = std::localtime(&now);

    std::array<char, 80> buffer;

    // Format: "10:25:02 Feb 20 2026"
    std::strftime(buffer.data(), buffer.size(), "%H:%M:%S %b %d %Y", ltm);

    return std::string(buffer.data());
}

Server::Server(const std::string &serverName, const std::string &password, int port)
    : _serverName(serverName),
      _password(password),
      _creationDate(getCurrentDate()),
      _port(port),
      _server_fd(-1)
{
    std::fill(_polls.begin(), _polls.end(), pollfd{});
}

Server::Server(const Server &other)
    : _serverName(other._serverName),
      _password(other._password),
      _creationDate(other._creationDate),
      _port(other._port),
      _server_fd(other._server_fd),
      _polls(other._polls)
{
}

Server &Server::operator=(const Server &other)
{
    if (this == &other)
        return *this;
    _serverName = other._serverName;
    _password = other._password;
    _creationDate = other._creationDate;
    _port = other._port;
    _server_fd = other._server_fd;
    _polls = other._polls;
    return *this;
}

Server::~Server()
{
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

pollfd &Server::getPollByFd(int fd)
{
    for (size_t i = 0; i < _polls.size(); i++)
    {
        if (_polls[i].fd == fd)
            return _polls[i];
    }
    throw ServerException("Pollfd with fd " + std::to_string(fd) + " not found");
}

void Server::init(int domain)
{
    // Creating the server socket
    // AF_INET (Ipv4), SOCK_STREAM (TCP), 0 (Default) => TCP
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    // SO_REUSEADDR allows immediately reconnection to port
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        throw ServerException("setsockopt(SO_REUSEADDR) failed");
    }

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
    std::cout << Color::MAGENTA << "server runnning on port " << _port << "..." << Color::RESET << std::endl;
}

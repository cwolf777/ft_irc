
#include "Server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password), _server_fd(-1)
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
            int client_fd = accept(_poll_fds[0].fd, nullptr, nullptr);
            if (client_fd >= 0)
            {
                std::cout << "new client connected!\n";
                _poll_fds.push_back({client_fd, POLLIN, 0});
                _clients.push_back(Client(client_fd, "", false, false));
            }
        }
        for (size_t i = 1; i < _poll_fds.size(); i++)
        {
            if (_poll_fds[i].revents & POLLIN)
            {
                char buffer[512];
                ssize_t bytes_recvd;
                bytes_recvd = recv(_poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes_recvd <= 0)
                {
                    std::cout << "Client disconnected!\n";
                    close(_poll_fds[i].fd);
                    _poll_fds.erase(_poll_fds.begin() + i);
                    _clients.erase(_clients.begin() + i);
                    i--;
                    continue;
                }
                if (bytes_recvd >= 512)
                {
                    std::cout << "SPAM!!! Client disconnected!\n";
                    close(_poll_fds[i].fd);
                    _poll_fds.erase(_poll_fds.begin() + i);
                    _clients.erase(_clients.begin() + i);
                    i--;
                    continue;
                }
                buffer[bytes_recvd] = '\0';
                std::string msg(buffer);
                std::cout << "Client: " << msg << "\n";
                IrcMsg request;
                try
                {
                    size_t pos = 0;
                    while ((pos = msg.find("\r\n")) != std::string::npos)
                    {

                        request.create(msg.substr(0, pos + 2));
                        std::cout << request << std::endl;
                        handle_request(_clients[i], request);
                        msg = msg.substr(pos + 2);
                    }
                }
                catch (const IrcMsg::IrcMsgException &e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
        }
    }
    close(_server_fd);
}

std::string Server::getPassword() const
{
    return _password;
}

void Server::setPassword(std::string pass)
{
    _password = pass;
}

bool Server::isNickUsed(const std::string &nick)
{
    for (auto it = _clients.begin(); it != _clients.end(); it++)
    {
        if (it->getNickname() == nick)
            return true;
    }
    return false;
}

const std::string Server::getOperatorPassword() const
{
    return _operatorPassword;
}

bool Server::isUsernameUsed(const std::string &username)
{
    for (auto it = _clients.begin(); it != _clients.end(); it++)
    {
        if (it->getUsername() == username)
            return true;
    }
    return false;
}

std::string Server::getOperatorName() const
{
    return _operatorName;
}

Channel Server::getChannel(const std::string &name) const
{
    for (size_t i = 0; i < _channel.size(); i++)
    {
        if (_channel[i].getName() == name)
            return _channel[i];
    }
    throw ServerException("");
}

Client Server::getClientByNick(const std::string nick) const
{
    for (auto it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->getNickname() == nick)
        {
            return (*it);
        }
    }
    throw ServerException("");
}

void Server::handle_request(const Client &client, const IrcMsg &request)
{
    if (request.get_cmd() == IRC_CAP)
    {
        std::cout << "Send to client\n";
        // _clients[i].sendMessage()
        IrcMsg response(":irc.34 CAP * LS :\r\n");
        send_response(client, response);
        // TODO: TEST
    }

    if (request.get_cmd() == IRC_NICK)
    {
        // client.set_nickname()
    }
}

void Server::send_response(const Client &client, const IrcMsg &response)
{
    send(client.getFd(), response.get_msg().c_str(), response.get_msg().size(), 0);
}

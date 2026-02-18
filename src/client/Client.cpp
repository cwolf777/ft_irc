
#include "Client.hpp"
#include "Channel.hpp"

static bool checkNickname(const std::string &nick)
{
    // allowed: letters: a-z, A-Z, numbers: 0-9, extras: - [ ] \ ^ { }`
    // firstChar must NOT be extra or number
    // length 1 - 9
    if (nick.empty())
        return false;

    if (nick.length() > 9)
        return false;

    if (!std::isalpha(nick[0]))
        return false;

    for (size_t i = 0; i < nick.size(); i++)
    {
        char c = nick[i];

        if (std::isalnum(c))
            continue;

        if (c == '-' || c == '[' || c == ']' ||
            c == '\\' || c == '`' || c == '^' ||
            c == '{' || c == '}')
            continue;

        return false;
    }

    return true;
}

Client::Client()
    : _fd(-1),
      _nickname(""),
      _username(""),
      _realName(""),
      _hostname(""),
      _hasNick(false),
      _hasUser(false),
      _hasPass(false),
      _isRegistered(false),
      _joinedChannels()
{
}

Client::Client(int fd)
    : _fd(fd),
      _nickname(""),
      _username(""),
      _realName(""),
      _hostname(""),
      _hasNick(false),
      _hasUser(false),
      _hasPass(false),
      _isRegistered(false),
      _joinedChannels(),
      _buffer("") {}

Client::Client(int fd, const std::string &hostname)
    : _fd(fd),
      _nickname(""),
      _username(""),
      _realName(""),
      _hostname(hostname),
      _hasNick(false),
      _hasUser(false),
      _hasPass(false),
      _isRegistered(false),
      _joinedChannels(),
      _buffer("") {}

Client::Client(const Client &other)
    : _fd(other._fd),
      _nickname(other._nickname),
      _username(other._username),
      _realName(other._realName),
      _hostname(other._hostname),
      _hasNick(other._hasNick),
      _hasUser(other._hasUser),
      _hasPass(other._hasPass),
      _isRegistered(other._isRegistered),
      _joinedChannels(other._joinedChannels),
      _buffer(other._buffer)

{
}

Client &Client::operator=(const Client &other)
{
    if (this == &other)
        return *this;

    _fd = other._fd;
    _nickname = other._nickname;
    _username = other._username;
    _realName = other._realName;
    _hostname = other._hostname;

    _hasNick = other._hasNick;
    _hasUser = other._hasUser;
    _hasPass = other._hasPass;
    _isRegistered = other._isRegistered;
    _joinedChannels = other._joinedChannels;
    _buffer = other._buffer;
    return *this;
}

// MOVE CONSTRUCTOR
// Client::Client(Client &&other) noexcept
//     : _fd(other._fd),
//       _nickname(std::move(other._nickname)),
//       _username(std::move(other._username)),
//       _realName(std::move(other._realName)),
//       _hostname(std::move(other._hostname)),
//       _hasNick(other._hasNick),
//       _hasUser(other._hasUser),
//       _hasPass(other._hasPass),
//       _isRegistered(other._isRegistered),
//       _joinedChannels(std::move(other._joinedChannels))
// {
//     other._fd = -1;
// }

// Client &Client::operator=(Client &&other) noexcept
// {
//     if (this == &other)
//         return *this;

//     if (_fd > 0)
//         close(_fd);

//     _fd = other._fd;
//     other._fd = -1;

//     _nickname = std::move(other._nickname);
//     _username = std::move(other._username);
//     _realName = std::move(other._realName);
//     _hostname = std::move(other._hostname);

//     _hasNick = other._hasNick;
//     _hasUser = other._hasUser;
//     _hasPass = other._hasPass;
//     _isRegistered = other._isRegistered;
//     _joinedChannels = std::move(other._joinedChannels);

//     return *this;
// }

// Fd will be closed safely in disconnectClient
Client::~Client()
{
    // if (_fd > 0)
    //     close(_fd);
}

int Client::getFd() const
{
    if (_fd == -1)
    {
        throw std::runtime_error("Attempted to use a Client with invalid fd!");
    }
    return _fd;
}

std::string Client::getNickname() const
{
    return _nickname;
}

std::string Client::getUsername() const
{
    return _username;
}

std::string Client::getRealname() const
{
    return _realName;
}

std::string Client::getHostname() const
{
    return _hostname;
}

bool Client::getIsRegistered() const
{
    return _isRegistered;
}

std::vector<Channel *> &Client::getChannels()
{
    return _joinedChannels;
}

std::string Client::getPrefix() const
{
    // Format: Nick!User@Host
    return _nickname + "!" + _username + "@" + _hostname;
}

std::string Client::getBuffer()
{
    return _buffer;
}

bool Client::hasNick() const
{
    return _hasNick;
}

bool Client::hasUser() const
{
    return _hasUser;
}

bool Client::hasPass() const
{
    return _hasPass;
}

void Client::joinChannel(Channel *channel)
{
    // if (!channel)
    //     return;
    for (Channel *c : _joinedChannels)
    {
        if (c->getName() == channel->getName())
            return; // TODO: EXCEPTION
    }
    _joinedChannels.push_back(channel);
}

void Client::setNickname(const std::string &nick)
{
    if (!checkNickname(nick))
        throw std::invalid_argument("Error invalid nickname"); // TODO: better exception
    _nickname = nick;
}

void Client::setUsername(const std::string &name)
{
    _username = name;
}

void Client::setRealname(const std::string &name)
{
    _realName = name;
}

void Client::setHasPass(bool flag)
{
    _hasPass = flag;
}

void Client::setHasNick(bool flag)
{
    _hasNick = flag;
}

void Client::setHasUser(bool flag)
{
    _hasUser = flag;
}

void Client::setBuffer(const std::string &buffer)
{
    if (buffer.size() > sizeof(char) * 512)
        throw std::overflow_error("Buffer overflow: incoming data exceeds buffer size");
    _buffer = buffer;
}

void Client::appendToBuffer(const std::string &data)
{
    if (_buffer.size() + data.size() > sizeof(char) * 512)
        throw std::overflow_error("Buffer overflow: incoming data exceeds buffer size");
    _buffer += data;
}

bool Client::canRegister()
{
    if (_isRegistered)
        return false;
    if (_hasPass && _hasNick && _hasUser)
    {
        _isRegistered = true;
        return true;
    }
    return false;
}

std::ostream &operator<<(std::ostream &os, const Client &client)
{
    os << "=== Client ===" << std::endl;

    os << "  FD (Socket):      " << client.getFd() << std::endl;
    os << "  Nickname:         " << client.getNickname() << std::endl;
    os << "  Username:         " << client.getUsername() << std::endl;

    os << "  Registration:     ";
    if (client.getIsRegistered())
    {
        os << "**REGISTERED** (Ready)";
    }
    else
    {
        os << "Pending";
    }
    os << std::endl;

    os << "===================" << std::endl;

    return os;
}
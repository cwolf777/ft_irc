
#include "Server.hpp"
#include <csignal>

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Error: Wrong number of arguments." << std::endl;
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    std::string password = argv[2];
    Server local("bucks34", password, port);
    try
    {
        local.init(INADDR_LOOPBACK);
        local.run();
        local.shutdown("Server shutting down");
    }
    catch (const std::exception &e)
    {
        std::cerr << Color::RED << "Server Error: " << e.what() << Color::RESET << std::endl;
        local.shutdown("Server shutting down due to error");
        return 1;
    }

    return 0;
}
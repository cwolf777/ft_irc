

#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void)
{

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0)
    {
        std::cerr << "ERROR: Socket could not be created!" << std::endl;
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // all Interfaces
    addr.sin_port = htons(6667);       // Port 6667

    if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "ERROR: Socket could not bin!" << std::endl;
        return 1;
    }
    if (listen(socket_fd, 5) < 0)
    {
        std::cerr << "Error: Socket could not listen\n";
        return 1;
    }

    std::cout << "server runnning on port 6667...\n";

    while (true)
    {
        int client_fd = accept(socket_fd, nullptr, nullptr);
        if (client_fd > 0)
        {
            std::cout << "connected!\n";
            close(client_fd);
        }
    }

    close(socket_fd);
    return 0;
}

#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <poll.h>

#include "User.hpp"

int main(void)
{

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        std::cerr << "ERROR: Socket could not be created!" << std::endl;
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // all Interfaces
    addr.sin_port = htons(6667);       // Port 6667

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "ERROR: Socket could not bind!" << std::endl;
        return 1;
    }
    if (listen(server_fd, 5) < 0)
    {
        std::cerr << "Error: Socket could not listen\n";
        return 1;
    }

    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    std::cout << "server runnning on port 6667...\n";

    std::vector<pollfd> fds;
    std::vector<User> users;

    fds.push_back({server_fd, POLLIN, 0});
    while (true)
    {
        poll(fds.data(), fds.size(), -1); // -1 blocking
        if (fds[0].revents & POLLIN)
        {
            int client_fd = accept(server_fd, nullptr, nullptr);
            if (client_fd >= 0)
            {
                std::cout << "new client connected!\n";
                fds.push_back({client_fd, POLLIN, 0});
            }
        }
        for (size_t i = 1; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                char buffer[1024];
                int n = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                if (n <= 0)
                {
                    std::cout << "Client disconnected!\n";
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    i--;
                }
                else
                {
                    buffer[n] = '\0';
                    std::string msg(buffer);
                    std::cout << "Client: " << msg << "\n";
                    if (msg == "CAP LS")
                    {
                        send(fds[i].fd, ":irc.34 CAP * LS :\r\n", 23, 0); // no CAP features
                        send(fds[i].fd, ":irc.34 001 nickname :Welcome to the irc chat, nickname!\r\n", 58, 0);
                    }
                    if (msg == "NICK jofmann")
                    {
                    }
                }
            }
        }
    }
    close(server_fd);
    return 0;
}
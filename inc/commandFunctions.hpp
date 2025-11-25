#pragma once
#include "Client.hpp"
#include <vector>
#include <string>
#include <sstream>

// Commands

void handlePass(Server &server, Client &client, const std::vector<std::string> &args);
void handleNick(Server &server, Client &client, const std::vector<std::string> &args);
void handleClient(Client &client, const std::vector<std::string> &args);
void handleOper(Server &server, Client &client, const std::vector<std::string> &args);
void handleMode(Server &server, Client &client, const std::vector<std::string> &args);
void handleQuit(Client &client, const std::vector<std::string> &args);
void handleJoin(Client &client, const std::vector<std::string> &args);
void handleTopic(Client &client, const std::vector<std::string> &args);
void handleKick(Client &client, const std::vector<std::string> &args);
void privMsg(Client &client, const std::vector<std::string> &args);

#pragma once
#include "Client.hpp"
#include "IrcMsg.hpp"

class Server;

void handleCap(Client &client, Server &server, const IrcMsg &msg);
void handleInvite(Client &client, Server &server, const IrcMsg &msg);
void handleJoin(Client &client, Server &server, const IrcMsg &msg);
void handleKick(Client &client, Server &server, const IrcMsg &msg);
void handleMode(Client &client, Server &server, const IrcMsg &msg);
void handleNames(Client &client, Server &server, const IrcMsg &msg);
void handleNick(Client &client, Server &server, const IrcMsg &msg);
void handlePart(Client &client, Server &server, const IrcMsg &msg);
void handlePass(Client &client, Server &server, const IrcMsg &msg);
void handlePing(Client &client, Server &server, const IrcMsg &msg);
void handlePrivMsg(Client &client, Server &server, const IrcMsg &msg);
void handleQuit(Client &client, Server &server, const IrcMsg &msg);
void handleTopic(Client &client, Server &server, const IrcMsg &msg);
void handleUser(Client &client, Server &server, const IrcMsg &msg);
void handleWho(Client &client, Server &server, const IrcMsg &msg);
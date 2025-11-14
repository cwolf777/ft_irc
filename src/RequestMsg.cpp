
#include "RequestMsg.hpp"

RequestMsg::RequestMsg() : _cmd(IRC_UNKNOWN), _org_msg("") {}

RequestMsg::RequestMsg(std::string msg) : _org_msg(msg)
{
    std::string cmd;
    std::stringstream ss(msg);
    ss >> cmd;

    auto it = IRC_COMMANDS.find(cmd);
    if (it != IRC_COMMANDS.end())
        _cmd = it->second;
    else
        _cmd = IRC_COMMANDS.at(IRC_UNKNOWN);
    // TODO: NEED TO STORE ARGS AND MAYBE IMPLEMENT CHECK FOR VALID ARGS
}
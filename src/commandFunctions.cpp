#include "commandFunctions.hpp"
#include "User.hpp"

void handlePass(User &user, const std::vector<std::string> &args)
{
    if (user.isRegistered())
    {
        user.sendMessage("462 :You may not reregister");
        return;
    }
    if (args.size() < 2)
    {
        user.sendMessage("461 PASS :Not enough parameters");
        return;
    }
    std::string pass = args[1];

    //if (pass != server.getPass())
    //{
    //  user.sendMessage("464 :Password incorrect");
    //  return;
    //}

    //noch nicht registriert, da Nick noch nicht gesetzt 
}

void handleNick(User &user, const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        user.sendMessage("431 :No nickname given");
        return;
    }
    // if (!checkNickname(args[1]))
    // {
    //     user.sendMessage("432 :Erroneous nickname");
    //     return;
    // }
    //checken ob Name bereits vergeben ist bei allen Usern in Server
    //wenn ok, nickname setten fuer user
}

void handleUser(User &user, const std::vector<std::string> &args)
{
    //USER <username> <hostname> <servername> :<realname>
    //check ich nciht
}

void handleOper(User &user, const std::vector<std::string> &args)
{
    //OPER <username> <password>
    //checken ob 3 in arg
    //checken ob User uebrhaupt exestiert
    //checken ob Operator PW uebereinstimmt
    //User zu Operator setten
    //msg senden
}

void handleMode(User &user, const std::vector<std::string> &args)
{
    //MODE <channel> <modes> [parameters]
    //kein plan
}

void handleQuit(User &user, const std::vector<std::string> &args)
{
    //braodcast to channel
    //remove User from channel
    //disconnect User
}

void handleJoin(User &user, const std::vector<std::string> &args)
{
    //check ob arg groesser als 2 
    //channle name muss mit # anfangen?
    //get Channel class falls nein erstelle neuen Channel als class
    //add user to channel
    //boradcast join

}

void handleTopic(User &user, const std::vector<std::string> &args)
{
    //arg muss groeser gleich 2 sein 
    // arg[1] = channel name
    //get channel object if not available return 
    //if arg == 2 
    //-> braodcast channel name or no channel topic if nothing set yet
    //if arg == 3 topic setzen (in channel struct) ((setter und getter func benutzen))

}

void handleKick(User &user, const std::vector<std::string> &args)
{
    //arg muss groesser gleich 3 sein
    //arg[1] channel name
    //arg[2] targetUser
    //get Channel if not return 
    //check if user (which want to kick) in channel isMember()
    //check if User is operaator in channel
    //getUser (target)
    //chekc if target is in channel
    //Grund zusammensetzen mit arg 3, etc falls mehr args
    //Target aus Channel entfernen 
}

void privMsg(User &user, const std::vector<std::string> &args)
{
    //arg muss groesser gleich 3 sein
    //arg[1] channel name
    //arg[2] targetUser
    //get Channel if not return 
    //check if user (which want to kick) in channel isMember()
    //check if User is operaator in channel
    //getUser (target)
    //chekc if target is in channel
    //Grund zusammensetzen mit arg 3, etc falls mehr args
    //Target aus Channel entfernen 
}
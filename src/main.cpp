
#include "Server.hpp"

int main(void)
{

    Server local(6667, "abcdef");
    local.init(INADDR_LOOPBACK);
    local.run();
}
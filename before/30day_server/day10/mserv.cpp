#include "inc/event_loop.h"
#include "inc/server.h"

int main() {
    auto *loop = new EventLoop();
    Server *server = new Server(loop);
    loop->Loop();

    delete server;
    delete loop;
    return 0;
}
#include "inc/event_loop.h"
#include "inc/server.h"

int main() {
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    loop->Loop();
    return 0;
}
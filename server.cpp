#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include "sockets/passive_socket.h"

using namespace std;

int main(int argc, char *argv[]) {
    passive_socket passiveSocket{10};

    active_socket client1{};
    active_socket client2{};
    active_socket client3{};
    active_socket client4{};

    if (passive_socket_init(&passiveSocket)) {

        if (passive_socket_bind(&passiveSocket)) {
            if (passive_socket_listen(&passiveSocket)) {
                if (passive_socket_wait_for_clients(&passiveSocket, &client1, 1) &&
                    passive_socket_wait_for_clients(&passiveSocket, &client2, 2)
                    && passive_socket_wait_for_clients(&passiveSocket, &client3, 3) &&
                    passive_socket_wait_for_clients(&passiveSocket, &client4, 4)) {
                    cout << endl;
                    cout << "All players has successfully joined. Game can start";
                }
            }
        }
    }

    return 0;
}
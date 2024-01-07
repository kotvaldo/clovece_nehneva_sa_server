#include <iostream>
#include "sockets/passive_socket.h"

using namespace std;


int main(int argc, char *argv[]) {
    passive_socket passiveSocket{10};
    int maxClients = 4;
    active_socket activeSockets[maxClients];


    passive_socket_init(&passiveSocket);

    for (int i = 0; i < maxClients; ++i) {
        active_socket_init(&activeSockets[i]);
        activeSockets[i].id = i + 1;
    }




    if (passive_socket_bind(&passiveSocket)) {
        if (passive_socket_listen(&passiveSocket)) {
            if (passive_socket_wait_for_clients(&passiveSocket, &activeSockets[0]) &&
                passive_socket_wait_for_clients(&passiveSocket, &activeSockets[1])
                && passive_socket_wait_for_clients(&passiveSocket, &activeSockets[2]) &&
                passive_socket_wait_for_clients(&passiveSocket, &activeSockets[3])) {
                cout << endl;
                cout << "All players has successfully joined. Game can start" << endl;
            }
        } else return 1;
    } else return 1;



    active_socket_write(&activeSockets[0], "vajico");
    active_socket_read(&activeSockets[0]);

    for (int i = 0; i < activeSockets[0].data.size(); ++i) {
        cout << activeSockets[0].data[i];
    }

    passive_socket_destroy(&passiveSocket);
    for (int i = 0; i < maxClients; ++i) {
        active_socket_destroy(&activeSockets[i]);
    }


}


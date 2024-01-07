#include <iostream>
#include "sockets/passive_socket.h"

using namespace std;


int main(int argc, char *argv[]) {
    passive_socket passiveSocket{10};

    active_socket client1 = {1};
    active_socket_init(&client1);
    passive_socket_init(&passiveSocket);
    if (passive_socket_bind(&passiveSocket)) {
        if (passive_socket_listen(&passiveSocket)) {
            if (passive_socket_wait_for_clients(&passiveSocket, &client1)){
                cout << "everything is done." << endl;
            }
        } else return 1;
    } else return 1;



    active_socket_read(&client1);
    for (int i = 0; i < client1.data.size(); ++i) {
        cout << client1.data[i];
    }
    active_socket_write(&client1, "vajico");
}


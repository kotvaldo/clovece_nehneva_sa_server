//
// Created by adams on 6. 1. 2024.
//


#include "passive_socket.h"

#define PORT 11000


bool passive_socket_init(struct passive_socket *passiveSocket) {
    bzero(reinterpret_cast<char *>(&passiveSocket->serv_addr), sizeof(passiveSocket->serv_addr));
    passiveSocket->serv_addr.sin_family = AF_INET;
    passiveSocket->serv_addr.sin_addr.s_addr = INADDR_ANY;
    passiveSocket->serv_addr.sin_port = htons(PORT);

    passiveSocket->socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (passiveSocket->socket_descriptor < 0) {
        cout << "Error creating socket" << endl;
        return false;
    } else {
        cout << "Passive socket was created and initialized." << endl;
    }
    pthread_mutex_init(&passiveSocket->mutex, NULL);
    pthread_cond_init(&passiveSocket->waiting_finished, NULL);
    return true;
}

void passive_socket_destroy(struct passive_socket *passiveSocket) {


}

bool passive_socket_listen(struct passive_socket *passiveSocket) {
    pthread_mutex_lock(&passiveSocket->mutex);
    if (passiveSocket->isListening || passiveSocket->isWaiting) {
        pthread_mutex_unlock(&passiveSocket->mutex);
        cout << "Passive socket is listening, you cannot start listening again!" << endl;
        return false;
    }
    listen(passiveSocket->socket_descriptor, passiveSocket->max_clients);
    passiveSocket->isListening = true;
    pthread_mutex_unlock(&passiveSocket->mutex);
    cout << "Listening started" << endl;
    return true;
}

bool passive_socket_bind(struct passive_socket *passiveSocket) {
    if (bind(passiveSocket->socket_descriptor, reinterpret_cast<struct sockaddr *>(&passiveSocket->serv_addr),
             sizeof(passiveSocket->serv_addr)) < 0) {
        cout << "bind() has failed !" << endl;
        return false;
    } else {
        cout << "bind() was successful !" << endl;
        return true;
    }
}

bool passive_socket_is_listening_now(struct passive_socket *passiveSocket) {
    pthread_mutex_lock(&passiveSocket->mutex);
    bool is_listening = passiveSocket->isListening;
    pthread_mutex_unlock(&passiveSocket->mutex);
    return is_listening;
}


bool passive_socket_wait_for_clients(struct passive_socket *passiveSocket, struct active_socket *activeSocket, int client) {
    pthread_mutex_lock(&passiveSocket->mutex);
    if (passiveSocket->isWaiting) {
        pthread_mutex_unlock(&passiveSocket->mutex);
        cout << "Passive socket is accepting clients, you cannost start again !" << endl;
        return false;
    }
    pthread_mutex_unlock(&passiveSocket->mutex);

    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);

    fd_set sockets;
    FD_ZERO(&sockets);
    struct timeval tv;
    tv.tv_usec = 0;
    cout << endl;
    cout << "Waiting for client "<< client << endl;
    while (passive_socket_is_listening_now(passiveSocket)) {
        FD_SET(passiveSocket->socket_descriptor, &sockets);
        tv.tv_sec = 1;

        select(passiveSocket->socket_descriptor + 1, &sockets, NULL, NULL, &tv);
        if (FD_ISSET(passiveSocket->socket_descriptor, &sockets)) {
            activeSocket->socket_descriptor = accept(passiveSocket->socket_descriptor,
                                                     reinterpret_cast<struct sockaddr *>(&client_address),
                                                     &client_address_length);
            if (activeSocket->socket_descriptor < 0) {
                cout << "Accept error" << endl;
                return false;
            }

            cout << "Client has been accepted" << endl;
            pthread_mutex_lock(&passiveSocket->mutex);
            passiveSocket->isWaiting = false;
            pthread_mutex_unlock(&passiveSocket->mutex);
            return true;
        }
    }

    passiveSocket->isWaiting = false;
    pthread_mutex_unlock(&passiveSocket->mutex);
    return true;
}


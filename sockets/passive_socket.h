//
// Created by adams on 6. 1. 2024.
//

#ifndef CLOVECE_NEHNEVA_SA_SERVER_PASSIVE_SOCKET_H
#define CLOVECE_NEHNEVA_SA_SERVER_PASSIVE_SOCKET_H

#include <pthread.h>
#include "active_socket.h"
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;
struct passive_socket {
    int max_clients;
    int socket_descriptor;
    bool isListening;
    bool isWaiting;
    pthread_mutex_t mutex;
    pthread_cond_t waiting_finished;
    sockaddr_in serv_addr;

};

bool passive_socket_init(struct passive_socket *passiveSocket);
void passive_socket_destroy(struct passive_socket *passiveSocket);
bool passive_socket_listen(struct passive_socket * passiveSocket);
bool passive_socket_bind(struct passive_socket *passiveSocket);
bool passive_socket_wait_for_clients(struct passive_socket *passiveSocket, struct active_socket *activeSocket, int client);


#endif //CLOVECE_NEHNEVA_SA_SERVER_PASSIVE_SOCKET_H

//
// Created by adams on 6. 1. 2024.
//

#ifndef CLOVECE_NEHNEVA_SA_SERVER_ACTIVE_SOCKET_H
#define CLOVECE_NEHNEVA_SA_SERVER_ACTIVE_SOCKET_H


#include <pthread.h>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


struct active_socket {
    int client_number;
    int socket_descriptor;
};


#endif //CLOVECE_NEHNEVA_SA_SERVER_ACTIVE_SOCKET_H

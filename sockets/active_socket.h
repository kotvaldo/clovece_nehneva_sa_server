//
// Created by adams on 6. 1. 2024.
//

#ifndef CLOVECE_NEHNEVA_SA_SERVER_ACTIVE_SOCKET_H
#define CLOVECE_NEHNEVA_SA_SERVER_ACTIVE_SOCKET_H

#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

struct active_socket {
    int id {};
    int socket_descriptor = -1;
    string end_message;
    bool is_reading = false;
    pthread_mutex_t mutex_reading;
    pthread_mutex_t mutex_writing;
    pthread_mutex_t mutex_received_data;
    vector<string> data;

};

void active_socket_init(struct active_socket *self);

void active_socket_destroy(struct active_socket *self);

void active_socket_read(struct active_socket *self);

void active_socket_stop_reading(struct active_socket *self);

bool active_socket_is_reading(struct active_socket *self);


bool active_socket_is_end_message(struct active_socket *self, string message);

void active_socket_write(struct active_socket *self, string message);

void active_socket_write_end_message(struct active_socket *self);

#endif //CLOVECE_NEHNEVA_SA_SERVER_ACTIVE_SOCKET_H

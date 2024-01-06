//
// Created by adams on 6. 1. 2024.
//

#ifndef CLOVECE_NEHNEVA_SA_SERVER_ACTIVE_SOCKET_H
#define CLOVECE_NEHNEVA_SA_SERVER_ACTIVE_SOCKET_H

#include <pthread.h>
#include "char_buffer.h"
#include "linked_list.h"

using namespace std;

GENERATE_LINKED_LIST(struct char_buffer, char_buffer);

typedef struct active_socket {
    int client_number;
    int socket_descriptor;
    const char* end_message;
    bool is_reading;
    pthread_mutex_t mutex_reading;
    pthread_mutex_t mutex_writing;
    LINKED_LIST_char_buffer received_data;
    pthread_mutex_t mutex_received_data;
};

void active_socket_init(struct active_socket* self);
void active_socket_destroy(struct active_socket* self);
void active_socket_start_reading(struct active_socket* self);
void active_socket_stop_reading(struct active_socket* self);
bool active_socket_is_reading(struct active_socket* self);
bool active_socket_try_get_read_data(struct active_socket* self, struct char_buffer* output);
bool active_socket_is_end_message(struct active_socket* self, struct char_buffer* message);

void active_socket_write_data(struct active_socket* self, struct char_buffer* message);
void active_socket_write_end_message(struct active_socket* self);

#endif //CLOVECE_NEHNEVA_SA_SERVER_ACTIVE_SOCKET_H

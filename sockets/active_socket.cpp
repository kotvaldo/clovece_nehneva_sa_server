
#include "active_socket.h"

#define INVALID_SOCKET -1
#define SOCKET_TERMINATE_CHAR '\0'
#define BUFFER_LENGTH 100

void active_socket_init(struct active_socket *self) {
    self->socket_descriptor = INVALID_SOCKET;
    self->end_message = ":end";
    self->is_reading = false;
    pthread_mutex_init(&self->mutex_reading, NULL);
    pthread_mutex_init(&self->mutex_writing, NULL);
    pthread_mutex_init(&self->mutex_received_data, NULL);
}

void active_socket_destroy(struct active_socket *self) {
    if (self->socket_descriptor >= 0) {
        close(self->socket_descriptor);
    }
    self->socket_descriptor = INVALID_SOCKET;

    self->is_reading = false;
    self->data.clear();
    self->buffer.clear();
    destroy(self->buffer.begin(), self->buffer.end());
    pthread_mutex_destroy(&self->mutex_reading);
    pthread_mutex_destroy(&self->mutex_writing);
    pthread_mutex_destroy(&self->mutex_received_data);
}

void active_socket_stop_reading(struct active_socket *self) {
    pthread_mutex_lock(&self->mutex_reading);
    self->is_reading = false;
    pthread_mutex_unlock(&self->mutex_reading);
}

bool active_socket_is_reading(struct active_socket *self) {
    pthread_mutex_lock(&self->mutex_reading);
    bool can_read = self->is_reading;
    pthread_mutex_unlock(&self->mutex_reading);
    return can_read;
}

bool active_socket_is_end_message(struct active_socket *self, string message) {
    return message == self->end_message && message.length() == self->end_message.length();
}

void active_socket_start_reading(struct active_socket *self) {
    pthread_mutex_lock(&self->mutex_reading);

    if (self->is_reading) {
        pthread_mutex_unlock(&self->mutex_reading);
        fprintf(stderr, "active_socket is reading. Cannot call start_reading again.\n");
        return;
    }

    self->is_reading = true;
    pthread_mutex_unlock(&self->mutex_reading);

    fd_set sockets;
    struct timeval tv;
    tv.tv_sec = 1;  // Nastavte vhodný čas čakania

    while (active_socket_is_reading(self)) {
        FD_ZERO(&sockets);
        FD_SET(self->socket_descriptor, &sockets);

        int result = select(self->socket_descriptor + 1, &sockets, NULL, NULL, &tv);

        if (result == -1) {
            perror("select failed.");
            active_socket_stop_reading(self);
            close(self->socket_descriptor);
            self->socket_descriptor = INVALID_SOCKET;
            break;
        }

        if (result > 0) {

            self->buffer.clear();

            ssize_t bytesRead = read(self->socket_descriptor, self->buffer.data(), self->buffer.size());

            if (bytesRead > 0) {
                size_t first_i = 0;
                size_t last_i = 0;
                stringstream ss;

                for (size_t i = 0; i < bytesRead; ++i) {
                    char ch = self->buffer[i];

                    if (ch == SOCKET_TERMINATE_CHAR) {
                        self->data.push_back(ss.str());
                        ss.str("");
                        active_socket_stop_reading(self);
                    } else {
                        ss << ch;
                    }
                }
            } else if (bytesRead == 0) {
                // Koniec vstupu (EOF)
                active_socket_stop_reading(self);
                break;
            } else {
                // bytesRead < 0 - chyba pri čítaní
                perror("read failed.");
                active_socket_stop_reading(self);
                break;
            }
        }
    }
}

void active_socket_write_data(struct active_socket *self, string message) {
    pthread_mutex_lock(&self->mutex_writing);
    self->buffer.clear();
    message.copy(self->buffer.data(), message.size());
    self->buffer.push_back(SOCKET_TERMINATE_CHAR);
    write(self->socket_descriptor, self->buffer.data(), self->buffer.size());
    pthread_mutex_unlock(&self->mutex_writing);
}

void active_socket_write_end_message(struct active_socket *self) {
    pthread_mutex_lock(&self->mutex_writing);
    self->buffer.clear();
    self->end_message.copy(self->buffer.data(), self->end_message.size());
    self->buffer.push_back(SOCKET_TERMINATE_CHAR);
    write(self->socket_descriptor, self->buffer.data(), self->buffer.size());

    pthread_mutex_unlock(&self->mutex_writing);
}

#undef INVALID_SOCKET
#undef SOCKET_TERMINATE_CHAR
#undef BUFFER_LENGTH


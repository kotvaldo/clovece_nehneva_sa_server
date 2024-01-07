cmake_minimum_required(VERSION 3.18.4)
project(clovece_nehneva_sa_server)

set(CMAKE_CXX_STANDARD 17)

add_executable(clovece_nehneva_sa_server server.cpp
        sockets/passive_socket.cpp
        sockets/passive_socket.h
        sockets/active_socket.cpp
        sockets/active_socket.h
        sever_buffer/buffer.h
        sever_buffer/char_buffer.h
        sever_buffer/char_buffer.cpp
        sever_buffer/linked_list.h
        sever_buffer/buffer.cpp
        sever_buffer/buffer.h
        buffers/buffer.cpp
        buffers/buffer.h
        char_buffer/char_buffer.cpp
        char_buffer/char_buffer.h)

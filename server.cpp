#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

    if (argc < 2) {
        cerr << "usage " << argv[0] << " port" << endl;
        return 1;
    }

    bzero(reinterpret_cast<char *>(&serv_addr), sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) {
        perror("Error binding socket address");
        return 2;
    }

    int max_connections = 4;
    int clients_connected = 0;

    listen(sockfd, max_connections);
    cli_len = sizeof(cli_addr);

    // Accept connections until max is reached
    while (clients_connected < max_connections) {
        newsockfd = accept(sockfd, reinterpret_cast<struct sockaddr *>(&cli_addr), &cli_len);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            return 3;
        }
        cout << "Clients connected: " << clients_connected + 1 << endl;

        clients_connected++;
    }

    // Play the game
    while (true) {
        cout << "Your turn to roll the dice" << endl;

        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);

        if (strcmp(n, "end") == 0) {
            close(newsockfd);
            clients_connected--;
            cout << "Client disconnected. Remaining clients: " << clients_connected << endl;

            if (clients_connected <= 0) {
                break;
            }
        } else {
            int rng = rand() % 6 + 1;
            const string msg = "You rolled " + to_string(rng);

            n = write(newsockfd, msg.c_str(), msg.length() + 1);
            if (n < 0) {
                perror("Error writing to socket");
                return 5;
            }
        }
    }

    close(sockfd);

    return 0;
}

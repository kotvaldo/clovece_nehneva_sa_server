#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include "sockets/passive_socket.h"

#define NUMBER_OF_PLAYERS 4
#define BOARD_SIZE 40

typedef struct hracia_doska {
    int *pocet_hrajucich_hracov;
    active_socket *hraci;
    int *tah_hraca;
    int pocet_policok;
    int *aktualne_pozicie_panacikov;
    bool *cesta;
    bool *domcek_1;
    bool *domcek_2;
    bool *domcek_3;
    bool *domcek_4;
} HRACIA_DOSKA;

typedef struct data_hraci {
    int id;
    active_socket *socket;
    int *pozicie_panacikov;
    int *prejdene_policka;
    bool vyhral;
    HRACIA_DOSKA *hracia_doska;
    pthread_mutex_t *mutex;
    pthread_cond_t *je_tah_hraca;
} DATA_HRACI;

using namespace std;

void *vykonaj_tah(void *data) {
    auto *hrac = (DATA_HRACI *) data;

    while (hrac->hracia_doska->tah_hraca != hrac->id) {
        pthread_cond_wait(hrac->je_tah_hraca, hrac->mutex);
    }

    pthread_mutex_lock(hrac->mutex);
    while (!hrac->vyhral && hrac->hracia_doska->pocet_hrajucich_hracov == NUMBER_OF_PLAYERS) {
        pthread_mutex_unlock(hrac->mutex);
        Sleep(3);
        cout << "Hrac (" << hrac->id << ") je na rade." << endl;
        active_socket_write_data(hrac->socket, "vypis");
        active_socket_start_reading(hrac->socket);

        int hod = rand() % 6 + 1;

        switch (hrac->socket->data.back()) {
            case "1":
                int panacikovia_na_doske = 0;
                for (int i = 0; i < 4; ++i) {
                    if (hrac->pozicie_panacikov[i] >= 0 &&
                        hrac->pozicie_panacikov[i] < hrac->hracia_doska->pocet_policok) {
                        panacikovia_na_doske++;
                    }
                }
                Sleep(3);
                cout << "Hrac (" << hrac->id << ") hodil " << hod << "." << endl;
                active_socket_write_data(hrac->socket, "moznosti");
                active_socket_start_reading(hrac->socket);

                switch(hrac->socket->data.back()) {
                    case "3":
                        // server sa opyta klienta, ktorym panacikom sa chce pohnut
                        active_socket_write_data(hrac->socket, "hod");
                        active_socket_start_reading(hrac->socket);
                        if (panacikovia_na_doske > 0) {
                            int panacik = stoi(hrac->socket->data.back() - 1);
                            hrac->hracia_doska->cesta[hrac->pozicie_panacikov[panacik]] = false;
                            hrac->pozicie_panacikov[panacik] += hod;
                            hrac->hracia_doska->cesta[hrac->pozicie_panacikov[panacik]] = true;
                            cout << "Hrac (" << hrac->id << ") sa pohol panacikom " << panacik << "." << endl;
                        } else {
                            cout << "Hrac (" << hrac->id << ") sa nema s kym pohnut." << endl;
                        }
                        break;

                    case "4":
                        active_socket_write_data(hrac->socket, "novy");
                        active_socket_start_reading(hrac->socket);
                        if (hod == 6) {
                            int pozicia = daj_startovacie_policko(hrac->id);
                            // ak je niekto na startovacom policku daneho hraca
                            if (hrac->hracia_doska->cesta[pozicia]) {
                                for (int i = 0; i < 16; ++i) {
                                    if (hrac->hracia_doska->aktualne_pozicie_panacikov[i] == pozicia) {

                                    }
                                }
                            }
                            for (int i = 0; i < 4; ++i) {
                                if (hrac->pozicie_panacikov[i] < 0) {
                                    hrac->pozicie_panacikov[i] = pozicia;
                                    break;
                                }
                            }

                            hrac->hracia_doska->cesta[pozicia] = true;
                            cout << "Hrac (" << hrac->id << ") pridava svojho panacika na startovacie policko." << endl;
                        } else {
                            cout << "Hrac (" << hrac->id << ") nemoze pridat panacika." << endl;
                        }
                        break;

                    default:

                        break;
                }
                // pripady so ziadnym panacikom na doske
                if (panacikovia_na_doske == 0 && hod < 6) {
                    cout << "Hrac (" << hrac->id << ") sa nema s kym pohnut." << endl;
                } else {
                    int pozicia = daj_startovacie_policko(hrac->id);
                    // ak je niekto na startovacom policku daneho hraca
                    if (hrac->hracia_doska->cesta[pozicia]) {
                        for (int i = 0; i < 16; ++i) {
                            if (hrac->hracia_doska->aktualne_pozicie_panacikov[i] == pozicia) {

                            }
                        }
                    }
                    hrac->pozicie_panacikov[0] = pozicia;
                    hrac->hracia_doska->cesta[pozicia] = true;
                    cout << "Hrac (" << hrac->id << ") pridava svojho panacika na startovacie policko." << endl;
                }

                if (panacikovia_na_doske > 0) {
                    // server sa opyta klienta, ktorym panacikom sa chce pohnut
                    active_socket_write_data(hrac->socket, "Zadajte cislo panacika: \n");
                    // klient posle feedback
                    active_socket_start_reading(hrac->socket);
                    int panacik = stoi(hrac->socket->data.back() - 1);
                    hrac->pozicie_panacikov[panacik] += hod;
                    cout << "Hrac (" << hrac->id << ") sa pohol panacikom " << panacik << "." << endl;
                }

                // prepnutie tahu hraca
                pthread_mutex_unlock(hrac->mutex);
                hrac->hracia_doska->tah_hraca = (hrac->hracia_doska->tah_hraca + 1) % 4 + 1;
                pthread_cond_broadcast(hrac->je_tah_hraca);
                pthread_mutex_lock(hrac->mutex);
                break;

            case "2":
                sockets_broadcast(&hrac->hracia_doska->hraci, "koniec");
                active_socket_destroy(hrac->socket);
                hrac->hracia_doska->pocet_hrajucich_hracov--;
                break;

            default:
                active_socket_write_data(hrac->socket, "Neplatna moznost!");
                break;
        }
    }

    pthread_mutex_unlock(hrac->mutex);
    return nullptr;
}

int daj_startovacie_policko(int id) {
    switch (id) {
        case 1:
            return 0;
        case 2:
            return 10;
        case 3:
            return 20;
        case 4:
            return 30;
        default:
            return -1;
    }
}

int main(int argc, char *argv[]) {
    passive_socket passiveSocket{10};
    active_socket clients[NUMBER_OF_PLAYERS];

    passive_socket_init(&passiveSocket);
    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        active_socket_init(&clients[i]);
    }

    if (passive_socket_bind(&passiveSocket)) {
        if (passive_socket_listen(&passiveSocket)) {
            if (passive_socket_wait_for_clients(&passiveSocket, &clients[0]) &&
                passive_socket_wait_for_clients(&passiveSocket, &clients[1]) &&
                passive_socket_wait_for_clients(&passiveSocket, &clients[2]) &&
                passive_socket_wait_for_clients(&passiveSocket, &clients[3]) {
                cout << endl << "All players has successfully joined. Game can start." << endl;
            }
        } else return 1;
    } else return 1;

    active_socket_read(&clients[0]);
    for (int i = 0; i < &clients[0].data.size(); ++i) {
        cout << &clients[0].data[i] << endl;
    }
    active_socket_write(&clients[0], "vajico");

    // herna logika
    srand(time(nullptr));

    // inicializacia mutexov a conditionov
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_t je_tah_hraca;
    pthread_cond_init(&je_tah_hraca, nullptr);

    // inicializacia hracej dosky
    bool cesta[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) {
        cesta[i] = false;
    }
    bool domcek[4];
    for (int i = 0; i < 4; ++i) {
        domcek[i] = false;
    }
    int *pozicie_panacikov[16];
    for (int i = 0; i < 16; ++i) {
        pozicie_panacikov[i] = -1;
    }

    HRACIA_DOSKA hracia_doska = {
            NUMBER_OF_PLAYERS, &clients, 1, BOARD_SIZE, pozicie_panacikov, cesta, domcek, domcek, domcek, domcek
    };

    // inicializacia dat hracov
    DATA_HRACI data_hraci[NUMBER_OF_PLAYERS];
    pthread_t hraci[NUMBER_OF_PLAYERS];

    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        data_hraci[i].id = i + 1;
        data_hraci[i].socket = clients[i];
        data_hraci[i].pozicie_panacikov = {-1, -1, -1, -1};
        data_hraci[i].prejdene_policka = {0, 0, 0, 0};
        data_hraci[i].vyhral = false;
        data_hraci[i].hracia_doska = hracia_doska;
        data_hraci[i].mutex = mutex;
        data_hraci[i].je_tah_hraca = je_tah_hraca;
        pthread_create(&hraci[i], nullptr, vykonaj_tah, &data_hraci[i]);
    }

    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        pthread_join(hraci[i], nullptr);
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&je_tah_hraca);

    return 0;
}

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include "sockets/passive_socket.h"
#include "doska/doska.h"
#include "pthread.h"

#define NUMBER_OF_PLAYERS 4
#define BOARD_SIZE 40
using namespace std;


typedef struct hracia_doska {
    int pocet_hrajucich_hracov;
    active_socket *hraci;
    int tah_hraca;
    int *aktualne_pozicie_panacikov;
    DOSKA_DATA *doska;
} HRACIA_DOSKA;

typedef struct data_hraci {
    int id;
    char target;
    active_socket *socket;
    int *pozicie_panacikov;
    int prejdene_policka[4];
    bool *domcek;
    bool vyhral;
    HRACIA_DOSKA *hracia_doska;
    pthread_mutex_t *mutex;
    pthread_cond_t *je_tah_hraca;
} DATA_HRACI;


void broadcast_message(struct active_socket *hraci, const string &message) {
    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        active_socket_write(&hraci[i], message);
    }
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

void zvladaj_ukon(int ukon, int panacikovia_na_doske, int hod, DATA_HRACI *hrac) {
    if (ukon == 3) {
        if (panacikovia_na_doske > 0) {
            int panacik = stoi(hrac->socket->data.back()) - 1;
            int povodnaPoloha = hrac->pozicie_panacikov[panacik];
            int novaPoloha = (hrac->pozicie_panacikov[panacik] + hod) % BOARD_SIZE;

            hrac->pozicie_panacikov[panacik] = novaPoloha;
            hrac->hracia_doska->aktualne_pozicie_panacikov[panacik + 4 * hrac->id] = novaPoloha;

            // ak panacik presiel celu cestu az do domceku
            if (hrac->prejdene_policka[panacik] > BOARD_SIZE) {
                hrac->domcek[panacik] = true;
                hrac->pozicie_panacikov[panacik] = 999;
                move_player(hrac->hracia_doska->doska, povodnaPoloha, 999, hrac->target);
                cout << "Panacik hraca (" << hrac->id << ") sa dostal do domceka." << endl;
            } else {
                move_player(hrac->hracia_doska->doska, povodnaPoloha, novaPoloha, hrac->target);
                cout << "Hrac (" << hrac->id << ") sa pohol panacikom " << panacik << "." << endl;
            }

            // aktualizacia pozicie panacika daneho hraca
            broadcast_message(hrac->hracia_doska->hraci,
                              serialize_doska(hrac->hracia_doska->doska));
        } else {
            cout << "Hrac (" << hrac->id << ") sa nema s kym pohnut." << endl;
        }
    } else if (ukon == 4) {
        int start = daj_startovacie_policko(hrac->id);

        // ak je niekto na startovacom policku daneho hraca, vyhod ho
        for (int i = 0; i < 16; ++i) {
            if (hrac->hracia_doska->aktualne_pozicie_panacikov[i] == start) {
                hrac->hracia_doska->aktualne_pozicie_panacikov[i] = -1;
                move_player(hrac->hracia_doska->doska, start, -1, hrac->target);

            }
        }
        for (int i = 0; i < 4; ++i) {
            if (hrac->pozicie_panacikov[i] < 0) {
                hrac->pozicie_panacikov[i] = start;
                hrac->hracia_doska->aktualne_pozicie_panacikov[i + 4 * hrac->id] = start;
                move_player(hrac->hracia_doska->doska, -1, start, hrac->target);
            }
        }
        broadcast_message(hrac->hracia_doska->hraci, serialize_doska(hrac->hracia_doska->doska));
        cout << "Hrac (" << hrac->id << ") pridava svojho panacika na startovacie policko." << endl;
    } else if (ukon == 5) {
        broadcast_message(hrac->hracia_doska->hraci, "koniec");
        active_socket_destroy(hrac->socket);
        hrac->hracia_doska->pocet_hrajucich_hracov--;

    }

}


void *vykonaj_tah(void *data) {
    auto *hrac = (DATA_HRACI *) data;

    broadcast_message(hrac->hracia_doska->hraci, serialize_doska(hrac->hracia_doska->doska));

    while (hrac->hracia_doska->tah_hraca != hrac->id) {
        pthread_cond_wait(hrac->je_tah_hraca, hrac->mutex);
    }

    pthread_mutex_lock(hrac->mutex);
    while (!hrac->vyhral && hrac->hracia_doska->pocet_hrajucich_hracov == NUMBER_OF_PLAYERS) {
        pthread_mutex_unlock(hrac->mutex);
        // aktualizacia pozicii panacikov, ak niekto vyhodil hracovi
        for (int i = 0; i < 4; ++i) {
            hrac->pozicie_panacikov[i] = hrac->hracia_doska->aktualne_pozicie_panacikov[i + 4 * hrac->id];
        }
        sleep(3);
        cout << "Hrac (" << hrac->id << ") je na rade." << endl;
        active_socket_write(hrac->socket, "vypis");
        active_socket_read(hrac->socket);

        int tah = stoi(hrac->socket->data.back());
        if (tah == 1) {

            int hod = rand() % 6 + 1;
            int panacikovia_na_doske = 0;
            for (int i = 0; i < 4; ++i) {
                if (hrac->pozicie_panacikov[i] >= 0 && hrac->pozicie_panacikov[i] < BOARD_SIZE) {
                    panacikovia_na_doske++;
                }
            }

            sleep(3);
            cout << "Hrac (" << hrac->id << ") hodil " << hod << "." << endl;

            if (hod < 6) {
                // server sa opyta klienta, ktorym panacikom sa chce pohnut
                active_socket_write(hrac->socket, "hod");
                active_socket_write(hrac->socket, to_string(hod));
                active_socket_read(hrac->socket);
                tah = stoi(hrac->socket->data.back());
                if (tah == 3) {
                    zvladaj_ukon(3, panacikovia_na_doske, hod, hrac);
                } else if (tah == 5) {
                    zvladaj_ukon(5, panacikovia_na_doske, hod, hrac);
                }
            } else {
                active_socket_write(hrac->socket, "hod_sestku");
                active_socket_write(hrac->socket, to_string(hod));
                active_socket_read(hrac->socket);
                tah = stoi(hrac->socket->data.back());
                if (tah == 3) {
                    zvladaj_ukon(3, panacikovia_na_doske, hod, hrac);
                } else if (tah == 4) {
                    zvladaj_ukon(4, panacikovia_na_doske, hod, hrac);
                } else if (tah == 5) {
                    zvladaj_ukon(5, panacikovia_na_doske, hod, hrac);
                }
            }

        } else if(tah == 5) {
            zvladaj_ukon(5, 0, 0, hrac);
        }

        int pocetVDomceku = 0;
        for (int i = 0; i < 4; ++i) {
            if (hrac->domcek) pocetVDomceku++;
        }
        if (pocetVDomceku == 4) {
            hrac->vyhral = true;
        }
        // prepnutie tahu hraca
        pthread_mutex_unlock(hrac->mutex);
        hrac->hracia_doska->tah_hraca = (hrac->hracia_doska->tah_hraca + 1) % 4 + 1;
        pthread_cond_broadcast(hrac->je_tah_hraca);
        pthread_mutex_lock(hrac->mutex);
    }
    pthread_mutex_unlock(hrac->mutex);
    return nullptr;
}

int main(int argc, char *argv[]) {
    passive_socket passiveSocket{10};
    active_socket clients[NUMBER_OF_PLAYERS];

    DOSKA_DATA doska;

    passive_socket_init(&passiveSocket);
    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        active_socket_init(&clients[i]);
    };

    if (passive_socket_bind(&passiveSocket)) {
        if (passive_socket_listen(&passiveSocket)) {
            if (passive_socket_wait_for_clients(&passiveSocket, &clients[0]) &&
                passive_socket_wait_for_clients(&passiveSocket, &clients[1]) &&
                passive_socket_wait_for_clients(&passiveSocket, &clients[2]) &&
                passive_socket_wait_for_clients(&passiveSocket, &clients[3])) {
                cout << endl << "All players has successfully joined. Game can start." << endl;
            }
        }
    }

    // herna logika
    srand(time(nullptr));

    // inicializacia mutexov a conditionov
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_t je_tah_hraca;
    pthread_cond_init(&je_tah_hraca, nullptr);

    // inicializacia hracej dosky

    int pozicie_panacikov[16];
    for (int i = 0; i < 16; ++i) {
        pozicie_panacikov[i] = -1;
    }

    HRACIA_DOSKA hracia_doska = {
            NUMBER_OF_PLAYERS, clients, 1, pozicie_panacikov, &doska
    };

    // inicializacia dat hracov
    DATA_HRACI data_hraci[NUMBER_OF_PLAYERS];
    pthread_t hraci[NUMBER_OF_PLAYERS];



    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        data_hraci[i].id = i;
        data_hraci[i].socket = &clients[i];
        for (int j = 0; j < 4; ++j) {
            data_hraci[i].prejdene_policka[j] = 0;
        }
        for (int j = 0; j < 4; ++j) {
            data_hraci[i].domcek[j] = false;
        }
        data_hraci[i].vyhral = false;
        data_hraci[i].hracia_doska = &hracia_doska;
        data_hraci[i].mutex = &mutex;
        data_hraci[i].je_tah_hraca = &je_tah_hraca;
        pthread_create(&hraci[i], nullptr, vykonaj_tah, &data_hraci[i]);
    }

    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        pthread_join(hraci[i], nullptr);
    }
    pthread_mutex_destroy(&mutex);

    return 0;
}
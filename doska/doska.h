//
// Created by adams on 8. 1. 2024.
//

#ifndef CLOVECE_NEHNEVA_SA_SERVER_DOSKA_H
#define CLOVECE_NEHNEVA_SA_SERVER_DOSKA_H
#include <iostream>
#include <vector>
using namespace std;

struct doska {
    char pole_field[11][11];
    vector<vector<int>> pole_suradnic;

};

void doska_initial(struct doska *doska);

void doska_destroy(struct doska *doska);

void print_doska(struct doska *doska);

string serialize_doska(struct doska *doska);

void move_player( struct doska *doska, int povodnaPoloha, int novaPoloha, char target);

#endif //CLOVECE_NEHNEVA_SA_SERVER_DOSKA_H

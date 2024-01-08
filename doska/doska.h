//
// Created by adams on 8. 1. 2024.
//

#ifndef CLOVECE_NEHNEVA_SA_SERVER_DOSKA_H
#define CLOVECE_NEHNEVA_SA_SERVER_DOSKA_H
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

typedef struct doska {
    char pole_field[11][11];
    std::vector<std::vector<int>> pole_suradnic = {
            {0,  6}, //zaciatok pre niekoho , 0
            {1,  6},
            {2,  6},
            {3,  6},
            {4,  6},
            {4,  7},
            {4,  8},
            {4,  9},
            {4,  10},
            {5,  10},
            {6,  10}, //zaciatok pre niekoho, 10
            {6,  9},
            {6,  8},
            {6,  7},
            {6,  6},
            {7,  6},
            {8,  6},
            {9,  6},
            {10, 6},
            {10, 5},
            {10, 4}, // zaciatok pre niekoho, 20
            {9,  4},
            {8,  4},
            {7,  4},
            {6,  4},
            {6,  3},
            {6,  2},
            {6,  1},
            {6,  0},
            {5,  0},
            {4,  0}, //zaciatok pre niekoho, 30
            {4,  1},
            {4,  2},
            {4,  3},
            {4,  4},
            {3,  4},
            {2,  4},
            {1,  4},
            {0,  4},
            {0,  5}
    };

}DOSKA_DATA;

void doska_initial(struct doska *doska);

void doska_destroy(struct doska *doska);

void print_doska(struct doska *doska);

string serialize_doska(struct doska *doska);

void move_player( struct doska *doska, int povodnaPoloha, int novaPoloha, char target);

#endif //CLOVECE_NEHNEVA_SA_SERVER_DOSKA_H

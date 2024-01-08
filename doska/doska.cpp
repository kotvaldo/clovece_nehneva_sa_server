//
// Created by adams on 8. 1. 2024.
//

#include "doska.h"
#define n 11


void doska_initial(struct doska *doska) {
    int npol = n / 2;
    doska->pole_suradnic = {
            {0,0},  // toto nikdy nedavaj, to je aby som ti zosynchronizoval pozicie od 1 po 40 , nie od 0 po 39
            {0,  6}, //zaciatok pre niekoho , 1
            {1,  6},
            {2,  6},
            {3,  6},
            {4,  6},
            {4,  7},
            {4,  8},
            {4,  9},
            {4,  10},
            {5,  10},
            {6,  10}, //zaciatok pre niekoho
            {6,  9},
            {6,  8},
            {6,  7},
            {6,  6},
            {7,  6},
            {8,  6},
            {9,  6},
            {10, 6},
            {10, 5},
            {10, 4}, // zaciatok pre niekoho
            {9,  4},
            {8,  4},
            {7,  4},
            {6,  4},
            {6,  3},
            {6,  2},
            {6,  1},
            {6,  0},
            {5,  0},
            {4,  0}, //zaciatok pre niekoho
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


    for (int i = 0; i < n; ++i) {
        doska->pole_field[npol - 1][i] = '*';
        doska->pole_field[npol + 1][i] = '*';
        doska->pole_field[i][npol - 1] = '*';
        doska->pole_field[i][npol + 1] = '*';
    }

    doska->pole_field[npol][0] = '*';
    doska->pole_field[npol][n - 1] = '*';
    doska->pole_field[0][npol] = '*';
    doska->pole_field[n - 1][npol] = '*';

    for (int i = 1; i < npol; ++i) {
        doska->pole_field[npol][i] = 'D';
        doska->pole_field[npol][n - i - 1] = 'D';
        doska->pole_field[i][npol] = 'D';
        doska->pole_field[n - i - 1][npol] = 'D';
    }

    doska->pole_field[npol][npol] = 'X';
}


void print_doska(struct doska **doska) {


}

string serialize_doska(struct doska *doska) {



}

void move_player( struct doska *doska, int povodnaPoloha, int novaPoloha, char target) {




}
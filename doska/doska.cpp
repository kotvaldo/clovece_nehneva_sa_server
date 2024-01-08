//
// Created by adams on 8. 1. 2024.
//

#include "doska.h"
#define n 11


void doska_initial(struct doska *doska) {
    int npol = n / 2;

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

void doska_destroy(struct doska *doska) {
    doska->pole_suradnic.clear();
    free(doska->pole_field);
}

void print_doska(struct doska *doska) {
    cout << endl;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << doska->pole_field[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;

}

string serialize_doska(struct doska *doska) {
    ostringstream ss;
    ss << endl;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            ss << doska->pole_field[i][j] << ' ';
        }
        ss << endl;
    }
    ss << endl;

    return ss.str();


}

void move_player(struct doska *doska, int povodnaPoloha, int novaPoloha, char target) {
    if (povodnaPoloha == -1) {
        doska->pole_field[doska->pole_suradnic[novaPoloha][0]][doska->pole_suradnic[novaPoloha][1]] = target;
    } else {
        doska->pole_field[doska->pole_suradnic[novaPoloha][0]][doska->pole_suradnic[novaPoloha][1]] = target;
        doska->pole_field[doska->pole_suradnic[povodnaPoloha][0]][doska->pole_suradnic[povodnaPoloha][1]] = '*';
    }
}
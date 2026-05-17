#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "file_handler.h"

// Funkcija za vizualni ispis ploče s obojanim znakovima
static void nacrtaj_plocu(char ploca[3][3]) {
    printf("\n      0   1   2 \n");
    for (int i = 0; i < 3; i++) {
        printf(" %d | ", i);
        for (int j = 0; j < 3; j++) {
            // Ako je na polju X, ispisujemo ga u crvenoj boji
            if (ploca[i][j] == 'X') {
                printf("\033[1;31mX\033[0m");
            }
            // Ako je na polju O, ispisujemo ga u plavoj boji
            else if (ploca[i][j] == 'O') {
                printf("\033[1;34mO\033[0m");
            }
            // Ako je polje prazno, ispisujemo običan razmak
            else {
                printf(" ");
            }
            
            printf(" | ");
        }
        printf("\n");
        if (i < 2) printf("   |---|---|---|\n");
    }
}

// Provjera pobjede (redovi, stupci i dijagonale)
static int provjeri_pobjednika(char ploca[3][3]) {
    for (int i = 0; i < 3; i++) {
        if (ploca[i][0] != ' ' && ploca[i][0] == ploca[i][1] && ploca[i][1] == ploca[i][2]) return 1;
        if (ploca[0][i] != ' ' && ploca[0][i] == ploca[1][i] && ploca[1][i] == ploca[2][i]) return 1;
    }
    if (ploca[0][0] != ' ' && ploca[0][0] == ploca[1][1] && ploca[1][1] == ploca[2][2]) return 1;
    if (ploca[0][2] != ' ' && ploca[0][2] == ploca[1][1] && ploca[1][1] == ploca[2][0]) return 1;
    return 0;
}

void play_game() {
    TrenutniRezultat res;
    char ploca[3][3] = { {' ',' ',' '}, {' ',' ',' '}, {' ',' ',' '} };
    int red, stup, potezi = 0;
    char trenutni_znak = 'X';

    printf("Ime igraca 1 (X): "); scanf("%s", res.player1);
    printf("Ime igraca 2 (O): "); scanf("%s", res.player2);

    while (potezi < 9) {
        nacrtaj_plocu(ploca);
        char* aktivan = (trenutni_znak == 'X') ? res.player1 : res.player2;

        // Kod ispisa teksta isto možemo obojati oznaku (%c) da igrač lakše vidi koji je znak
        if (trenutni_znak == 'X') {
            printf("Igrac %s (\033[1;31mX\033[0m), unesi red i stupac (npr. 0 1): ", aktivan);
        } else {
            printf("Igrac %s (\033[1;34mO\033[0m), unesi red i stupac (npr. 0 1): ", aktivan);
        }
        
        if (scanf("%d %d", &red, &stup) != 2) {
            printf("Greska pri unosu! Koristi brojeve.\n");
            while (getchar() != '\n'); 
            continue;
        }

        if (red < 0 || red > 2 || stup < 0 || stup > 2 || ploca[red][stup] != ' ') {
            printf("Nevazeci potez!\n");
            continue;
        }

        ploca[red][stup] = trenutni_znak;
        potezi++;

        if (provjeri_pobjednika(ploca)) {
            nacrtaj_plocu(ploca);
            strcpy(res.winner, aktivan);
            printf("Pobjeda: %s!\n", res.winner);
            save_result(&res); 
            return;
        }
        trenutni_znak = (trenutni_znak == 'X') ? 'O' : 'X';
    }
    
    printf("Izjednaceno!\n");
    strcpy(res.winner, "Izjednaceno");
    save_result(&res);
}

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "file_handler.h"

#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

// [Koncept 5: Konzistentno imenovanje - snake_case za funkcije i varijable]
// [Koncept 6: Primjena kljucne rijeci static za funkcije]
// [Koncept 12: Umjesto VLA polja, koristi se cisti pokazivac na pokazivac (char**)]
// [Savjet 2: Funkcija mora biti kratka i konkretna - samo crta plocu]
static void nacrtaj_plocu(char** ploca) {
    system("cls"); // Cisti konzolu radi vizualnog osvjezavanja

    printf("\n\033[1;36m=== KRIZIC KRUZIC ===\033[0m\n");
    printf("\n      0   1   2 \n");
    for (int i = 0; i < 3; i++) {
        printf(" %d \033[1;33m|\033[0m ", i);
        for (int j = 0; j < 3; j++) {
            // Pristup preko pokazivaca: ploca[i][j] je isto sto i *(*(ploca + i) + j)
            if (ploca[i][j] == 'X') {
                printf("\033[1;31mX\033[0m");
            }
            else if (ploca[i][j] == 'O') {
                printf("\033[1;34mO\033[0m");
            }
            else {
                printf(" ");
            }

            printf(" \033[1;33m|\033[0m ");
        }
        printf("\n");
        if (i < 2) printf("   \033[1;33m|---|---|---|\033[0m\n");
    }
    printf("\n");
}

// [Koncept 6: Primjena kljucne rijeci static za funkcije]
// [Koncept 12: Prosljedivanje pokazivaca na pokazivac za provjeru matrice]
static int provjeri_pobjednika(char** ploca) {
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
    int red, stup, potezi = 0;
    char trenutni_znak = 'X';

    // [Koncept 16: Dinamicko zauzimanje memorije za dvodimenzionalno polje preko char**]
    // Prvo alociramo niz od 3 pokazivaca na char (retke)
    char** ploca = (char**)malloc(3 * sizeof(char*));
    if (ploca == NULL) {
        perror("Greska pri alokaciji redaka ploce");
        return;
    }
    // Zatim za svaki redak alociramo 3 stupca i inicijaliziramo ih na prazno mjesto ' '
    for (int i = 0; i < 3; i++) {
        ploca[i] = (char*)malloc(3 * sizeof(char));
        if (ploca[i] == NULL) {
            perror("Greska pri alokaciji stupaca ploce");
            for (int j = 0; j < i; j++) free(ploca[j]);
            free(ploca);
            return;
        }
        for (int j = 0; j < 3; j++) {
            ploca[i][j] = ' ';
        }
    }

    system("cls");
    printf("\033[1;33m--- UNOS IGRACA ---\033[0m\n");
    printf("Player name 1 (\033[1;31mX\033[0m): "); scanf("%s", res.player1);
    printf("Player name 2 (\033[1;34mO\033[0m): "); scanf("%s", res.player2);

    while (potezi < 9) {
        nacrtaj_plocu(ploca);
        char* aktivan = (trenutni_znak == 'X') ? res.player1 : res.player2;

        if (trenutni_znak == 'X') {
            printf("Igrac \033[1;31m%s (X)\033[0m, unesi red i stupac (npr. 0 1): ", aktivan);
        }
        else {
            printf("Igrac \033[1;34m%s (O)\033[0m, unesi red i stupac (npr. 0 1): ", aktivan);
        }

        if (scanf("%d %d", &red, &stup) != 2) {
            printf("\033[1;31mGreska pri unosu! Koristi brojeve.\033[0m\n");
            while (getchar() != '\n');
            SLEEP_MS(1500);
            continue;
        }

        if (red < 0 || red > 2 || stup < 0 || stup > 2 || ploca[red][stup] != ' ') {
            printf("\033[1;31mNevazeci potez ili zauzeto polje!\033[0m\n");
            SLEEP_MS(1200);
            continue;
        }

        ploca[red][stup] = trenutni_znak;
        potezi++;

        if (provjeri_pobjednika(ploca)) {
            nacrtaj_plocu(ploca);
            strcpy(res.winner, aktivan);

            printf("\n\033[1;32m***************************************\033[0m\n");
            printf("\033[1;32m     POBJEDNIK JE: %s!          \033[0m\n", res.winner);
            printf("\033[1;32m***************************************\033[0m\n");

            // [Koncept 12: Generalna upotreba pokazivaca]
            save_result(&res);

            // [Koncept 17: Obavezno oslobadanje dinamicki alocirane 2D matrice s free()]
            for (int i = 0; i < 3; i++) free(ploca[i]);
            free(ploca);
            // [Koncept 18: Sigurno anuliranje pokazivaca na NULL]
            ploca = NULL;
            return;
        }
        trenutni_znak = (trenutni_znak == 'X') ? 'O' : 'X';
    }

    nacrtaj_plocu(ploca);
    printf("\n\033[1;33m***************************************\033[0m\n");
    printf("\033[1;33m          IZJEDNACENO!                 \033[0m\n");
    printf("\033[1;33m***************************************\033[0m\n");
    strcpy(res.winner, "Izjednaceno");
    save_result(&res);

    // [Koncept 17: Obavezno oslobadanje dinamicki alocirane 2D matrice kod izjednacenog ishoda]
    for (int i = 0; i < 3; i++) free(ploca[i]);
    free(ploca);
    // [Koncept 18: Sigurno anuliranje pokazivaca na NULL]
    ploca = NULL;
}

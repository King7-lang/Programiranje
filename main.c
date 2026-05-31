#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "file_handler.h"

int main() {
    int input;

    // Inicijalizacija izbornika; koristimo MenuOpcija (enum) radi jasnoce koda.
    MenuOpcija choice = CREATE_INSERT;

    // Glavna petlja programa koja osigurava da se program izvrsava dok korisnik ne odabere IZLAZ.
    do {

        printf("\n\033[1;33m=====================================\033[0m\n");
        printf("\033[1;36m         KRIZIC KRUZIC           \033[0m\n");
        printf("\033[1;33m=====================================\033[0m\n");
        printf("\033[1;32m 1.\033[0m New game\n");
        printf("\033[1;32m 2.\033[0m Ranking overview\n");
        printf("\033[1;32m 3.\033[0m Update player\n");
        printf("\033[1;32m 4.\033[0m Delete player\n");
        printf("\033[1;31m 0.\033[0m Exit\n");
        printf("\033[1;33m-------------------------------------\033[0m\n");
        printf("Choice: ");

        // Robustan unos podataka: provjera je li korisnik unio broj; 
        // ako nije, cistimo ulazni spremnik (buffer) kako bismo sprijecili beskonacnu petlju.
        if (scanf("%d", &input) != 1) {
            while (getchar() != '\n');
            continue;
        }

        choice = (MenuOpcija)input;

        // Upravljacka logika: delegira zadatke na druge datoteke (game.c ili file_handler.c) 
        // ovisno o odabiru korisnika.
        switch (choice) {
        case CREATE_INSERT:
            play_game();
            break;

        case READ_STATS:
            system("cls");
            display_results();
            break;

        case UPDATE_RECORD: {
            char ime_za_update[50];
            int broj_pobjeda;

            printf("\nUnesi ime igraca kojem zelis izmijeniti statistiku: ");
            scanf("%s", ime_za_update);

            printf("Unesi NOVI ukupan broj pobjeda za ovog igraca: ");
            scanf("%d", &broj_pobjeda);

            // Poziv CRUD operacije azuriranja podataka.
            rucni_update_pobjeda(ime_za_update, broj_pobjeda);
            break;
        }

        case DELETE_PLAYER:
            system("cls");
            display_results();
            int idx;
            printf("\nUnesi redni broj igraca za brisanje: ");
            scanf("%d", &idx);

            // Poziv CRUD operacije brisanja zapisa.
            delete_single_result(idx);
            break;

        case EXIT:
            printf("\n\033[1;31mIzlaz iz programa...\033[0m\n");
            break;

        default:
            printf("\n\033[1;31mNevazeci odabir! Pokusajte ponovo.\033[0m\n");
            break;
        }

    } while (choice != EXIT);

    return 0;
}

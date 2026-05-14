#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "file_handler.h"

// Definiranje enumeracije za opcije izbornika
// IZLAZ dobiva vrijednost 0, NOVA_IGRA 1, i tako dalje
typedef enum {
    IZLAZ = 0,
    NEW_GAME,
    RANKING_OVERVIEW,
    DELETE_PLAYER
} MenuOpcija;

int main() {
    int input;
    MenuOpcija choice;

    do {
        printf("\n--- KRIZIC KRUZIC ---\n");
        printf("%d. New game\n", NEW_GAME);
        printf("%d. Pregled ljestvice\n", RANKING_OVERVIEW);
        printf("%d. Delete player\n", DELETE_PLAYER);
        printf("%d. Exit\n", IZLAZ);
        printf("Choice: ");

   
        if (scanf("%d", &input) != 1) {
            while (getchar() != '\n');
            continue;
        }

        // Cast-anje (pretvorba) int unosa u našu MenuOpcija enumeraciju
        choice = (MenuOpcija)input;

        switch (choice) {
        case NEW_GAME:
            play_game();
            break;

        case RANKING_OVERVIEW:
            display_results();
            break;

        case DELETE_PLAYER:
            display_results();
            int idx;
            printf("Unesi redni broj igraca za brisanje: ");
            scanf("%d", &idx);
            delete_single_result(idx);
            break;

        case IZLAZ:
            printf("Izlaz iz programa...\n");
            break;

        default:
            printf("Nevazeci odabir! Pokusajte ponovo.\n");
            break;
        }

    } while (choice != IZLAZ);

    return 0;
}
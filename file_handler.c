#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"

typedef enum { GUBITAK, POBJEDA, REMI } Ishod;

// Pomocna funkcija za azuriranje postojeceg zapisa u datoteci
static void azuriraj_ili_dodaj(const char* ime, Ishod ishod) {
    FILE* fp = fopen("igraci.bin", "rb+");
    IgracStatistika temp;
    int pronaden = 0;

    if (fp != NULL) {
        while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {
            if (strcmp(temp.ime, ime) == 0) {
                temp.odigrano++;
                if (ishod == POBJEDA) temp.pobjede++;
                else if (ishod == GUBITAK) temp.porazi++;

                fseek(fp, -(long)sizeof(IgracStatistika), SEEK_CUR);
                fwrite(&temp, sizeof(IgracStatistika), 1, fp);
                pronaden = 1;
                break;
            }
        }
        fclose(fp);
    }

    if (!pronaden) {
        fp = fopen("igraci.bin", "ab");
        if (fp != NULL) {
            strcpy(temp.ime, ime);
            temp.odigrano = 1;
            temp.pobjede = (ishod == POBJEDA) ? 1 : 0;
            temp.porazi = (ishod == GUBITAK) ? 1 : 0;
            fwrite(&temp, sizeof(IgracStatistika), 1, fp);
            fclose(fp);
        }
    }
}

void save_result(const TrenutniRezultat* res) {
    if (res == NULL) return;
    Ishod ishod1, ishod2;

    if (strcmp(res->winner, "Izjednaceno") == 0) {
        ishod1 = ishod2 = REMI;
    }
    else if (strcmp(res->winner, res->player1) == 0) {
        ishod1 = POBJEDA; ishod2 = GUBITAK;
    }
    else {
        ishod1 = GUBITAK; ishod2 = POBJEDA;
    }

    azuriraj_ili_dodaj(res->player1, ishod1);
    azuriraj_ili_dodaj(res->player2, ishod2);
}

void display_results() {
    FILE* fp = fopen("igraci.bin", "rb");
    if (fp == NULL) {
        printf("Nema spremljenih podataka.\n");
        return;
    }
    IgracStatistika temp;
    int i = 1;
    printf("\n--- LJESTVICA IGRACA ---\n");
    printf("Br. | %-15s | Pobjede | Porazi | Odigrano\n", "Ime");
    printf("------------------------------------------------------\n");
    while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {
        printf("%d.  | %-15s | %7d | %6d | %8d\n", i++, temp.ime, temp.pobjede, temp.porazi, temp.odigrano);
    }
    fclose(fp);
}

void delete_single_result(int index) {
    FILE* fp = fopen("igraci.bin", "rb");
    if (fp == NULL) return;

    IgracStatistika* data = NULL;
    int count = 0;
    IgracStatistika temp;

    while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {
        IgracStatistika* ptr = (IgracStatistika*)realloc(data, (count + 1) * sizeof(IgracStatistika));
        if (ptr) { data = ptr; data[count++] = temp; }
    }
    fclose(fp);

    if (index >= 1 && index <= count) {
        fp = fopen("igraci.bin", "wb");
        for (int i = 0; i < count; i++) {
            if (i == index - 1) continue;
            fwrite(&data[i], sizeof(IgracStatistika), 1, fp);
        }
        fclose(fp);
        printf("Igrac obrisan.\n");
    }
    free(data);
}
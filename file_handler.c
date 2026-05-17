#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"

typedef enum { GUBITAK, POBJEDA, REMI } Ishod;

// Pomocna funkcija za azuriranje – dodan 'const' za zastitu parametra (Stavka 14)
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
        if (fp == NULL) {
            perror("Greska pri otvaranju datoteke za dodavanje"); // Upravljanje pogreskama (Stavka 22)
            return;
        }
        strcpy(temp.ime, ime);
        temp.odigrano = 1;
        temp.pobjede = (ishod == POBJEDA) ? 1 : 0;
        temp.porazi = (ishod == GUBITAK) ? 1 : 0;
        fwrite(&temp, sizeof(IgracStatistika), 1, fp);
        fclose(fp);
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

// Funkcija usporedbe za qsort – sortira silazno prema broju pobjeda (Stavka 23 i 26)
static int usporedi_pobjede(const void* a, const void* b) {
    const IgracStatistika* igracA = (const IgracStatistika*)a;
    const IgracStatistika* igracB = (const IgracStatistika*)b;
    return (igracB->pobjede - igracA->pobjede);
}

void display_results() {
    FILE* fp = fopen("igraci.bin", "rb");
    if (fp == NULL) {
        printf("Nema spremljenih podataka o igracima.\n");
        return;
    }

    // Dinamicki ucitavamo sve igrace u memoriju kako bismo ih mogli sortirati
    IgracStatistika* ljestvica = NULL;
    IgracStatistika temp;
    int broj_igraca = 0;

    while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {
        IgracStatistika* ptr = (IgracStatistika*)realloc(ljestvica, (broj_igraca + 1) * sizeof(IgracStatistika));
        if (ptr == NULL) {
            perror("Greska pri alokaciji memorije");
            free(ljestvica);
            fclose(fp);
            return;
        }
        ljestvica = ptr;
        ljestvica[broj_igraca++] = temp;
    }
    fclose(fp);

    if (broj_igraca == 0) {
        printf("Datoteka je prazna.\n");
        return;
    }

    // Poziv ugradene qsort funkcije za sortiranje ljestvice (Stavka 23)
    qsort(ljestvica, broj_igraca, sizeof(IgracStatistika), usporedi_pobjede);

    printf("\n--- LJESTVICA IGRACA (SORTIRANO PO POBJEDAMA) ---\n");
    printf("Br. | %-15s | Pobjede | Porazi | Odigrano\n", "Ime");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < broj_igraca; i++) {
        printf("%d.  | %-15s | %7d | %6d | %8d\n",
            i + 1, ljestvica[i].ime, ljestvica[i].pobjede, ljestvica[i].porazi, ljestvica[i].odigrano);
    }

    free(ljestvica);
    ljestvica = NULL; // Sigurno nuliranje pokazivaca (Stavka 18)
}

void delete_single_result(int index) {
    FILE* fp = fopen("igraci.bin", "rb");
    if (fp == NULL) {
        perror("Greska pri otvaranju datoteke za brisanje");
        return;
    }

    IgracStatistika* data = NULL;
    int count = 0;
    IgracStatistika temp;

    while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {
        IgracStatistika* ptr = (IgracStatistika*)realloc(data, (count + 1) * sizeof(IgracStatistika));
        if (ptr == NULL) {
            perror("Greska pri realokaciji");
            free(data);
            fclose(fp);
            return;
        }
        data = ptr;
        data[count++] = temp;
    }
    fclose(fp);

    if (index >= 1 && index <= count) {
        fp = fopen("igraci.bin", "wb");
        if (fp != NULL) {
            for (int i = 0; i < count; i++) {
                if (i == index - 1) continue;
                fwrite(&data[i], sizeof(IgracStatistika), 1, fp);
            }
            fclose(fp);
            printf("Igrac uspjesno obrisan.\n");
        }
        else {
            perror("Greska pri upisivanju nakon brisanja");
        }
    }
    else {
        printf("Nevazeci redni broj!\n");
    }

    free(data);
    data = NULL; // Sigurno nuliranje pokazivaca (Stavka 18)
}

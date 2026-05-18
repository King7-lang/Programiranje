#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"

// [Koncept 4: Primjena typedef s enum tipovima]
typedef enum { GUBITAK, POBJEDA, REMI } Ishod;

// [Koncept 16: Koristiti dinamicko zauzimanje memorije za slozene tipove]
static IgracStatistika* ucitaj_sve_igrace(int* broj_igraca) {
    FILE* fp = fopen("igraci.bin", "rb");
    if (!fp) return NULL;

    IgracStatistika* ljestvica = NULL, temp;
    *broj_igraca = 0;

    while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {
        // [Koncept 17: Koristiti ugradenu funkciju realloc()]
        IgracStatistika* ptr = (IgracStatistika*)realloc(ljestvica, (*broj_igraca + 1) * sizeof(IgracStatistika));
        if (!ptr) {
            perror("Greska pri alokaciji");
            free(ljestvica);
            fclose(fp);
            return NULL;
        }
        ljestvica = ptr;
        ljestvica[(*broj_igraca)++] = temp;
    }
    fclose(fp);
    return ljestvica;
}

// [Koncept 6: Primjena kljucne rijeci static]
// [Koncept 14: Osiguranje parametara koristenjem kljucne rijeci const]
static void azuriraj_ili_dodaj(const char* ime, Ishod ishod) {
    // [Koncept 19: Datoteke - otvaranje binarne datoteke u rb+ modu]
    FILE* fp = fopen("igraci.bin", "rb+");
    IgracStatistika temp;
    int pronaden = 0;

    // [Koncept 19: Obavezna provjera pokazivaca na datoteku prije koristenja]
    if (fp != NULL) {
        while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {
            if (strcmp(temp.ime, ime) == 0) {
                temp.odigrano++;
                if (ishod == POBJEDA) temp.pobjede++;
                else if (ishod == GUBITAK) temp.porazi++;

                // [Koncept 20: Koristiti funkciju fseek za pozicioniranje u datoteci]
                fseek(fp, -(long)sizeof(IgracStatistika), SEEK_CUR);
                fwrite(&temp, sizeof(IgracStatistika), 1, fp);
                pronaden = 1;
                break;
            }
        }
        fclose(fp); // [Koncept 19: Obavezno zatvaranje datoteke]
    }

    // [Koncept 1: C & I u CRUID - Kreiranje novog zapisa ako ne postoji u bazi]
    if (!pronaden) {
        if (!(fp = fopen("igraci.bin", "ab"))) {
            perror("Greska pri otvaranju datoteke");
            return;
        }
        strcpy(temp.ime, ime);
        temp.odigrano = 1;
        temp.pobjede = (ishod == POBJEDA);
        temp.porazi = (ishod == GUBITAK);
        fwrite(&temp, sizeof(IgracStatistika), 1, fp);
        fclose(fp);
    }
}

// [Koncept 14: Zastita parametara strukture koristenjem kljucne rijeci const]
void save_result(const TrenutniRezultat* res) {
    if (!res) return;
    int izjednaceno = (strcmp(res->winner, "Izjednaceno") == 0);
    int p1_pobjednik = (strcmp(res->winner, res->player1) == 0);

    azuriraj_ili_dodaj(res->player1, izjednaceno ? REMI : (p1_pobjednik ? POBJEDA : GUBITAK));
    azuriraj_ili_dodaj(res->player2, izjednaceno ? REMI : (p1_pobjednik ? GUBITAK : POBJEDA));
}

// [Koncept 14: Parametri funkcije usporedbe su osigurani preko const void*]
static int usporedi_pobjede(const void* a, const void* b) {
    return (((const IgracStatistika*)b)->pobjede - ((const IgracStatistika*)a)->pobjede);
}

void display_results() {
    int broj_igraca;
    IgracStatistika* ljestvica = ucitaj_sve_igrace(&broj_igraca);

    if (!ljestvica || broj_igraca == 0) {
        printf("\033[1;31mNema spremljenih podataka ili je datoteka prazna.\033[0m\n");
        return;
    }

    // [Koncept 23: Sortiranje – obavezna primjena ugradene qsort() funkcije]
    // [Koncept 26: Pokazivaci na funkcije - predavanje funkcije kao argumenta]
    qsort(ljestvica, broj_igraca, sizeof(IgracStatistika), usporedi_pobjede);

    printf("\n\033[1;34m------------------------------------------------------\033[0m\n");
    printf("\033[1;36m       LJESTVICA IGRACA (SORTIRANO PO POBJEDAMA)      \033[0m\n");
    printf("\033[1;34m------------------------------------------------------\033[0m\n");
    printf("Br. | %-15s | Pobjede | Porazi | Odigrano\n", "Ime");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < broj_igraca; i++) {
        printf("\033[1;32m%2d.\033[0m  | %-15s | %7d | %6d | %8d\n",
            i + 1, ljestvica[i].ime, ljestvica[i].pobjede, ljestvica[i].porazi, ljestvica[i].odigrano);
    }
    printf("\033[1;34m------------------------------------------------------\033[0m\n");

    free(ljestvica);
    // [Koncept 18: Sigurno brisanje memorije - anuliranje pokazivaca na NULL]
    ljestvica = NULL;
}

void delete_single_result(int index) {
    int count;
    IgracStatistika* data = ucitaj_sve_igrace(&count);

    if (!data) {
        printf("Nema podataka za brisanje.\n");
        return;
    }

    // [Koncept 1: D u CRUID - Potpuno brisanje odabranog zapisa]
    if (index >= 1 && index <= count) {
        FILE* fp = fopen("igraci.bin", "wb");
        if (fp != NULL) {
            for (int i = 0; i < count; i++) {
                if (i == index - 1) continue;
                fwrite(&data[i], sizeof(IgracStatistika), 1, fp);
            }
            fclose(fp);
            printf("\033[1;32mIgrac uspjesno obrisan s ljestvice.\033[0m\n");
        }
        else {
            perror("Greska pri upisu nakon brisanja");
        }
    }
    else {
        printf("\033[1;31mNevazeci redni broj!\033[0m\n");
    }

    free(data);
    // [Koncept 18: Anuliranje pokazivaca na NULL]
    data = NULL;
}


// [Koncept 14: Osiguranje parametra const char* ime]
void rucni_update_pobjeda(const char* ime, int nove_pobjede) {
    FILE* fp = fopen("igraci.bin", "rb+");
    if (!fp) {
        printf("\033[1;31mDatoteka ne postoji ili se ne moze otvoriti.\033[0m\n");
        return;
    }

    IgracStatistika temp;
    int pronaden = 0;

    while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {
        if (strcmp(temp.ime, ime) == 0) {
            temp.pobjede = nove_pobjede;
            if (temp.pobjede > temp.odigrano) {
                temp.odigrano = temp.pobjede + temp.porazi;
            }

            // [Koncept 20: Koristiti funkciju fseek za vracanje pozicije unatrag]
            fseek(fp, -(long)sizeof(IgracStatistika), SEEK_CUR);
            fwrite(&temp, sizeof(IgracStatistika), 1, fp);
            pronaden = 1;
            printf("\033[1;32mStatistika za igraca '%s' je rucno postavljena na %d pobjeda!\033[0m\n", ime, nove_pobjede);
            break;
        }
    }

    if (!pronaden) printf("\033[1;31mIgrac '%s' nije pronaden u bazi.\033[0m\n", ime);
    fclose(fp);
}

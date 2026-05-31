#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"

// Koristim enumeraciju za jasno razlikovanje ishoda igre, 
// što kod cini citljivijim i manje podloznim greskama u usporedbama.
typedef enum { GUBITAK, POBJEDA, REMI } Ishod;

// Funkcija ucitava podatke iz binarne datoteke u dinamicki alociran niz struktura.
// Koristim 'static' kako bih ogranicio vidljivost funkcije na ovu datoteku (enkapsulacija).
static IgracStatistika* ucitaj_sve_igrace(int* broj_igraca) {
    FILE* fp = fopen("igraci.bin", "rb"); // Otvaram datoteku u binarnom modu za sigurno citanje struktura.
    if (!fp) return NULL;

    IgracStatistika* ljestvica = NULL, temp;
    *broj_igraca = 0;

    // Dinamicki alociram memoriju (realloc) kako bih mogao ucitati proizvoljan broj igraca 
    // koji se nalaze u bazi, cime osiguravam fleksibilnost programa.
    while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {

        IgracStatistika* ptr = (IgracStatistika*)realloc(ljestvica, (*broj_igraca + 1) * sizeof(IgracStatistika));
        if (!ptr) {
            // U slucaju neuspjele alokacije, cistim memoriju i zatvaram datoteku kako ne bi bilo curenja.
            perror("Greska pri alokaciji");
            free(ljestvica);
            fclose(fp);
            return NULL;
        }
        ljestvica = ptr;
        ljestvica[(*broj_igraca)++] = temp;
    }
    fclose(fp); // Obavezno zatvaranje datoteke radi oslobadanja resursa operacijskog sustava.
    return ljestvica;
}

static void azuriraj_ili_dodaj(const char* ime, Ishod ishod) {
    // Implementiram CRUD princip: trazim zapis u datoteci za azuriranje, 
    // a ako ga nema, kreiram novi zapis na kraju datoteke.
    FILE* fp = fopen("igraci.bin", "rb+");
    IgracStatistika temp;
    int pronaden = 0;

    if (fp != NULL) {
        while (fread(&temp, sizeof(IgracStatistika), 1, fp)) {
            if (strcmp(temp.ime, ime) == 0) {
                temp.odigrano++;
                if (ishod == POBJEDA) temp.pobjede++;
                else if (ishod == GUBITAK) temp.porazi++;

                // Pomocu fseek pomicem pokazivac unutar datoteke tocno na pocetak pronadenog zapisa,
                // sto mi omogucuje efikasno prepisivanje starog podatka novim bez stvaranja kopije datoteke.
                fseek(fp, -(long)sizeof(IgracStatistika), SEEK_CUR);
                fwrite(&temp, sizeof(IgracStatistika), 1, fp);
                pronaden = 1;
                break;
            }
        }
        fclose(fp);
    }

    if (!pronaden) {
        // Ako igrac ne postoji, otvaram datoteku u append modu ("ab") za dodavanje novog igraca.
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

// Parametar 'res' je oznacen kao 'const' kako bih osigurao da funkcija 
// samo cita podatke iz strukture bez opasnosti da ih nehotice izmijenim.
void save_result(const TrenutniRezultat* res) {
    if (!res) return;
    int izjednaceno = (strcmp(res->winner, "Izjednaceno") == 0);
    int p1_pobjednik = (strcmp(res->winner, res->player1) == 0);

    azuriraj_ili_dodaj(res->player1, izjednaceno ? REMI : (p1_pobjednik ? POBJEDA : GUBITAK));
    azuriraj_ili_dodaj(res->player2, izjednaceno ? REMI : (p1_pobjednik ? GUBITAK : POBJEDA));
}

// Koristim pokazivace na funkciju unutar qsort-a kako bih definirao 
// vlastito pravilo sortiranja (po broju pobjeda).
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

    // Pozivam ugradenu qsort funkciju za brzo sortiranje niza podataka u memoriji.
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

    // Nakon prikaza, eksplicitno oslobadam alociranu memoriju i postavljam pokazivac na NULL
    // kako bih izbjegao "visece" pokazivace (dangling pointers).
    free(ljestvica);
    ljestvica = NULL;
}

void delete_single_result(int index) {
    int count;
    IgracStatistika* data = ucitaj_sve_igrace(&count);

    if (!data) {
        printf("Nema podataka za brisanje.\n");
        return;
    }

    // Implementacija brisanja iz binarne datoteke: kreiram novu datoteku, 
    // prepisem sve podatke osim onog kojeg zelim obrisati.
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
    data = NULL;
}

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

            // Azuriram zapis na izvornoj poziciji u binarnoj datoteci.
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

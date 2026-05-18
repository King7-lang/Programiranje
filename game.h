#pragma once

typedef enum {
    IZLAZ = 0,
    CREATE_INSERT,   // C & I: Pokretanje nove igre i insert novih igraca u bazu
    READ_STATS,      // R: Citanje i pregled ljestvice s qsortom
    UPDATE_RECORD,   // U: Rucno azuriranje/izmjena statistike nekog igraca
    DELETE_PLAYER    // D: Brisanje igraca iz datoteke
} MenuOpcija;

typedef struct {
    char ime[50];
    int pobjede;     // [Koncept 2: Primitivni tipovi podataka - cijeli brojevi]
    int porazi;
    int odigrano;
} IgracStatistika;

typedef struct {
    char player1[50];
    char player2[50];
    char winner[50];
} TrenutniRezultat;

void play_game();

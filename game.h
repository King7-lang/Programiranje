#pragma once

// Enumeracija definira jasna imena za opcije izbornika, 
// cime se izbjegava koristenje "magicnih brojeva" i kod postaje citljiviji.
typedef enum {
    EXIT = 0,
    CREATE_INSERT,
    READ_STATS,
    UPDATE_RECORD,
    DELETE_PLAYER
} MenuOpcija;

// Struktura IgracStatistika cuva podatke o pojedinom igracu;
// koristimo fiksni niz za ime radi jednostavnosti spremanja u binarnu datoteku.
typedef struct {
    char ime[50];
    int pobjede;
    int porazi;
    int odigrano;
} IgracStatistika;

// Struktura za privremeno pracenje rezultata trenutne partije 
// prije nego se podaci trajno pohrane u datoteku.
typedef struct {
    char player1[50];
    char player2[50];
    char winner[50];
} TrenutniRezultat;

// Deklaracija glavne funkcije igre; koristenje ovakvog header-a 
// omogucuje modularnost i povezivanje s main.c datotekom.
void play_game();

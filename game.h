
#ifndef GAME_H
#define GAME_H



typedef struct {
    char ime[50];
    int pobjede;
    int porazi;
    int odigrano;
} IgracStatistika;

typedef struct {
    char player1[50];
    char player2[50];
    char winner[50];
} TrenutniRezultat;

void play_game();

#endif
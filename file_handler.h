#pragma once

// Header guard osigurava da se datoteka ukljuci samo jednom tijekom kompilacije, 
// sprjecavajuci pogreske visestruke definicije.
#include "game.h"

// Funkcije ispod cine CRUD sucelje. Koristenje 'const' pokazivaca jamci 
// da funkcije nece nehoticno mijenjati izvorne podatke (sigurnost podataka).

// Spremanje rezultata partije u binarnu datoteku.
void save_result(const TrenutniRezultat* res);

// citanje svih zapisa iz datoteke i prikaz ljestvice korisniku.
void display_results();

// Brisanje zapisa iz datoteke na temelju rednog broja (indeksa).
void delete_single_result(int index);

// Azuriranje broja pobjeda za odabranog igraca direktno u bazi.
void rucni_update_pobjeda(const char* ime, int nove_pobjede);

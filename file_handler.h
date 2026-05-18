#pragma once

#include "game.h"

// [Koncept 7: Organizacija izvornog koda - odvajanje prototipova u header]
void save_result(const TrenutniRezultat* res);
void display_results();
void delete_single_result(int index);
void rucni_update_pobjeda(const char* ime, int nove_pobjede);

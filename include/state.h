/* state.h - Data global aplikasi (daftar PC, token, paket). */
#ifndef STATE_H
#define STATE_H

#include "config.h"
#include "types.h"

extern komputer    pc[JUMLAH_PC];
extern Token       daftar_token[MAKS_TOKEN];
extern int         jumlah_token;
extern const Paket paket_list[];
extern const int   jumlah_paket;

/* Mengosongkan semua PC dan token, mengisi id PC. */
void init_data(void);

#endif

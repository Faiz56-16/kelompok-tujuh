/* state.c - Definisi data global dan daftar paket. */
#include <string.h>
#include "state.h"

komputer pc[JUMLAH_PC];
Token    daftar_token[MAKS_TOKEN];
int      jumlah_token = 0;

/* Tambah/ubah paket di sini. */
const Paket paket_list[] = {
    { "Paket test 1 Menit",      60,    100 },
    { "Paket 1 Jam",           3600,   5000 },
    { "Paket 2 Jam",           7200,   9000 },
    { "Paket 3 Jam",          10800,  13000 },
};
const int jumlah_paket = (int)(sizeof(paket_list) / sizeof(paket_list[0]));

void init_data(void) {
    int i;
    memset(pc, 0, sizeof(pc));
    memset(daftar_token, 0, sizeof(daftar_token));
    jumlah_token = 0;
    for (i = 0; i < JUMLAH_PC; i++) pc[i].id = i + 1;
}

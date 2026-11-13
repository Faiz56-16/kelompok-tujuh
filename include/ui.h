/* ui.h - Tampilan menu/tabel dan helper pemilihan unit. */
#ifndef UI_H
#define UI_H

#include "types.h"

void tampil_menu(void);
void tampil_status(void);

/* Indeks PC terpilih, atau -1 jika batal / tidak ada.
 * want_terpakai: 0 = unit KOSONG, 1 = unit TERPAKAI */
int  pilih_unit(int want_terpakai);
void baca_nama(komputer *k);

#endif

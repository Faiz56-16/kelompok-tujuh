/* rental.c - Alur sewa reguler (post-paid) dan penghentian sesi. */
#include <stdio.h>
#include <time.h>
#include "config.h"
#include "state.h"
#include "util.h"
#include "ui.h"
#include "billing.h"
#include "storage.h"
#include "rental.h"

void sewa_postpaid(void) {
    int idx = pilih_unit(0);
    komputer *k;
    char waktu[32];
    if (idx < 0) {
        printf("  Tidak ada unit yang dipilih.\n");
        return;
    }
    k = &pc[idx];
    baca_nama(k);
    if (input_habis() || k->nama_pelanggan[0] == '\0') return;

    k->terpakai = 1;
    k->mode = MODE_POSTPAID;
    k->waktu_mulai = time(NULL);
    k->durasi_paket = 0;
    k->harga_paket = 0;
    k->peringatan = 0;

    format_waktu(k->waktu_mulai, waktu, sizeof(waktu));
    printf("\n  PC-%02d di-UNLOCK untuk %s.\n", k->id, k->nama_pelanggan);
    printf("  Billing dimulai: %s\n", waktu);
    simpan_data();
}

void stop_sewa(void) {
    int idx = pilih_unit(1);
    if (idx < 0) {
        printf("  Tidak ada unit yang dipilih.\n");
        return;
    }
    selesai_sesi(&pc[idx], 0);
    simpan_data();
}

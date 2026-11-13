/* ui.c - Tampilan menu/tabel dan helper pemilihan unit. */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "state.h"
#include "util.h"
#include "billing.h"
#include "ui.h"

#define GARIS "  +-------+----------+--------------------+-----------+------------+----------------+\n"

void tampil_menu(void) {
    char tarif[32];
    format_rupiah(TARIF_PER_JAM, tarif, sizeof(tarif));
    printf("\n==============================================\n");
    printf("        RENTAL WARNET - MENU UTAMA\n");
    printf("        Tarif reguler: %s/jam\n", tarif);
    printf("==============================================\n");
    printf("  1. Lihat Status komputer (Billing Monitor)\n");
    printf("  2. Sewa komputer (Post-Paid / Reguler)\n");
    printf("  3. Selesai / Cetak Struk (Stop Billing)\n");
    printf("  4. Buat Token Paket (Kasir, Pre-Paid)\n");
    printf("  5. Login dengan Token (Pre-Paid)\n");
    printf("  6. Lihat Token Aktif\n");
    printf("  0. Keluar\n");
    printf("----------------------------------------------\n");
}

void tampil_status(void) {
    time_t now = time(NULL);
    int i;
    printf("\n" GARIS);
    printf("  | Unit  | Status   | Pelanggan          | Mode      | Terpakai   | Sisa / Estimasi|\n");
    printf(GARIS);
    for (i = 0; i < JUMLAH_PC; i++) {
        const komputer *k = &pc[i];
        if (!k->terpakai) {
            printf("  | PC-%02d | KOSONG   | %-18s | %-9s | %-10s | %-14s |\n",
                   k->id, "-", "-", "-", "-");
        } else {
            long detik = (long)difftime(now, k->waktu_mulai);
            char durasi[16], ket[40], rp[32];
            format_durasi(detik, durasi, sizeof(durasi));
            if (k->mode == MODE_PREPAID) {
                char sisa[16];
                format_durasi(k->durasi_paket - detik, sisa, sizeof(sisa));
                snprintf(ket, sizeof(ket), "Sisa %s", sisa);
            } else {
                format_rupiah(hitung_biaya_postpaid(detik), rp, sizeof(rp));
                snprintf(ket, sizeof(ket), "Est. %s", rp);
            }
            printf("  | PC-%02d | TERPAKAI | %-18.18s | %-9s | %-10s | %-14.14s |\n",
                   k->id, k->nama_pelanggan,
                   k->mode == MODE_PREPAID ? "Pre-Paid" : "Post-Paid",
                   durasi, ket);
        }
    }
    printf(GARIS);
}

int pilih_unit(int want_terpakai) {
    int i, ada = 0, nomor;
    printf("\n  Unit %s:", want_terpakai ? "TERPAKAI" : "KOSONG");
    for (i = 0; i < JUMLAH_PC; i++) {
        if (pc[i].terpakai == want_terpakai) {
            printf(" PC-%02d", pc[i].id);
            ada = 1;
        }
    }
    if (!ada) {
        printf(" (tidak ada)\n");
        return -1;
    }
    printf("\n");

    for (;;) {
        nomor = baca_int("  Pilih nomor PC (0 = batal): ", 0, JUMLAH_PC);
        if (input_habis() || nomor == 0) return -1;
        if (pc[nomor - 1].terpakai == want_terpakai) return nomor - 1;
        printf("  PC-%02d %s. Pilih unit lain.\n", nomor,
               want_terpakai ? "sedang KOSONG" : "sedang TERPAKAI");
    }
}

void baca_nama(komputer *k) {
    char nama[50];
    char *p;
    for (;;) {
        baca_baris("  Nama pelanggan: ", nama, sizeof(nama));
        if (input_habis()) { nama[0] = '\0'; break; }
        for (p = nama; *p; p++) if (*p == '|') *p = ' ';   /* aman untuk file data */
        if (nama[0] != '\0') break;
        printf("  Nama tidak boleh kosong.\n");
    }
    strcpy(k->nama_pelanggan, nama);
}

/* token.c - Paket dan token pre-paid (buat, login, daftar). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "config.h"
#include "state.h"
#include "util.h"
#include "ui.h"
#include "storage.h"
#include "token.h"

static void tampil_paket(void) {
    int i;
    char rp[32];
    printf("\n  Daftar paket:\n");
    for (i = 0; i < jumlah_paket; i++) {
        format_rupiah(paket_list[i].harga, rp, sizeof(rp));
        printf("   %d. %-20s %s\n", i + 1, paket_list[i].nama, rp);
    }
}

static void bersihkan_token_terpakai(void) {
    int i, j = 0;
    for (i = 0; i < jumlah_token; i++)
        if (!daftar_token[i].dipakai) daftar_token[j++] = daftar_token[i];
    jumlah_token = j;
}

static int token_ada(const char *kode) {
    int i;
    for (i = 0; i < jumlah_token; i++)
        if (strcmp(daftar_token[i].kode, kode) == 0) return 1;
    return 0;
}

void buat_token(void) {
    static const char charset[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789"; // char random untuk token
    int p, i;
    char kode[16], rp[32];

    tampil_paket();
    p = baca_int("  Pilih paket (0 = batal): ", 0, jumlah_paket);
    if (input_habis() || p == 0) return;

    if (jumlah_token >= MAKS_TOKEN) bersihkan_token_terpakai();
    if (jumlah_token >= MAKS_TOKEN) {
        printf("  Daftar token penuh. Gunakan token yang ada terlebih dahulu.\n");
        return;
    }

    do {
        strcpy(kode, "WN-");
        for (i = 0; i < 6; i++)
            kode[3 + i] = charset[rand() % (int)(sizeof(charset) - 1)];
        kode[9] = '\0';
    } while (token_ada(kode));

    strcpy(daftar_token[jumlah_token].kode, kode);
    daftar_token[jumlah_token].paket = p - 1;
    daftar_token[jumlah_token].dipakai = 0;
    jumlah_token++;

    format_rupiah(paket_list[p - 1].harga, rp, sizeof(rp));
    printf("\n  Token berhasil dibuat.\n");
    printf("  Kode  : %s\n", kode);
    printf("  Paket : %s\n", paket_list[p - 1].nama);
    printf("  Bayar : %s (terima dari pelanggan di kasir)\n", rp);
    simpan_data();
}

void login_token(void) {
    char kode[32];
    int i, t = -1, idx;
    komputer *k;

    baca_baris("  Masukkan kode token (kosong = batal): ", kode, sizeof(kode));
    if (input_habis() || kode[0] == '\0') return;
    for (i = 0; kode[i]; i++) kode[i] = (char)toupper((unsigned char)kode[i]);

    for (i = 0; i < jumlah_token; i++)
        if (strcmp(daftar_token[i].kode, kode) == 0) { t = i; break; }

    if (t < 0) {
        printf("  [X] Token tidak ditemukan.\n");
        return;
    }
    if (daftar_token[t].dipakai) {
        printf("  [X] Token sudah pernah dipakai.\n");
        return;
    }

    idx = pilih_unit(0);
    if (idx < 0) {
        printf("  Login dibatalkan (token belum terpakai).\n");
        return;
    }
    k = &pc[idx];
    baca_nama(k);
    if (input_habis() || k->nama_pelanggan[0] == '\0') return;

    k->terpakai = 1;
    k->mode = MODE_PREPAID;
    k->waktu_mulai = time(NULL);
    k->durasi_paket = paket_list[daftar_token[t].paket].durasi;
    k->harga_paket = paket_list[daftar_token[t].paket].harga;
    k->peringatan = 0;
    daftar_token[t].dipakai = 1;

    printf("\n  Token valid. PC-%02d di-UNLOCK untuk %s.\n", k->id, k->nama_pelanggan);
    printf("  %s - timer mundur dimulai (%d menit).\n",
           paket_list[daftar_token[t].paket].nama, k->durasi_paket / 60);
    simpan_data();
}

void lihat_token(void) {
    int i, ada = 0;
    char rp[32];
    printf("\n  Token aktif (belum dipakai):\n");
    for (i = 0; i < jumlah_token; i++) {
        if (daftar_token[i].dipakai) continue;
        format_rupiah(paket_list[daftar_token[i].paket].harga, rp, sizeof(rp));
        printf("   %-10s %-20s %s\n", daftar_token[i].kode,
               paket_list[daftar_token[i].paket].nama, rp);
        ada = 1;
    }
    if (!ada) printf("   (tidak ada)\n");
}

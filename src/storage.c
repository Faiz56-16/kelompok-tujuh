/* storage.c - Simpan/muat sesi dan token.
 *
 * Format baris:
 *   PC|id|terpakai|mode|detik_terpakai|durasi_paket|harga_paket|nama
 *   TK|kode|paket|dipakai
 *
 * Yang disimpan adalah detik TERPAKAI (bukan timestamp), sehingga saat
 * dimuat ulang timer melanjutkan sisa waktu tanpa menghitung waktu mati.
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "state.h"
#include "storage.h"

void simpan_data(void) {
    FILE *f = fopen(FILE_DATA, "w");
    time_t now = time(NULL);
    int i;
    if (!f) {
        printf("  [!] Gagal menyimpan data ke %s\n", FILE_DATA);
        return;
    }
    for (i = 0; i < JUMLAH_PC; i++) {
        long terpakai = pc[i].terpakai ? (long)difftime(now, pc[i].waktu_mulai) : 0;
        fprintf(f, "PC|%d|%d|%d|%ld|%d|%d|%s\n",
                pc[i].id, pc[i].terpakai, pc[i].mode, terpakai,
                pc[i].durasi_paket, pc[i].harga_paket, pc[i].nama_pelanggan);
    }
    for (i = 0; i < jumlah_token; i++)
        fprintf(f, "TK|%s|%d|%d\n",
                daftar_token[i].kode, daftar_token[i].paket, daftar_token[i].dipakai);
    fclose(f);
}

static void muat_baris_pc(const char *line, time_t now, int *aktif) {
    int id, terpakai, mode, durasi, harga;
    long berlalu;
    char nama[50] = "";
    int n = sscanf(line, "PC|%d|%d|%d|%ld|%d|%d|%49[^\n]",
                            &id, &terpakai, &mode, &berlalu, &durasi, &harga, nama);
    komputer *k;

    if (n < 6 || id < 1 || id > JUMLAH_PC) return;
    k = &pc[id - 1];
    k->terpakai = terpakai ? 1 : 0;
    k->mode = (mode == MODE_PREPAID) ? MODE_PREPAID : MODE_POSTPAID;
    k->durasi_paket = durasi;
    k->harga_paket = harga;
    snprintf(k->nama_pelanggan, sizeof(k->nama_pelanggan), "%s", n >= 7 ? nama : "");
    if (k->terpakai) {
        k->waktu_mulai = now - (time_t)berlalu;   /* lanjutkan sisa waktu */
        (*aktif)++;
    } else {
        k->waktu_mulai = 0;
    }
}

static void muat_baris_token(const char *line)
{
    char kode[16];
    int paket, dipakai;
    if (jumlah_token >= MAKS_TOKEN) return;
    if (sscanf(line, "TK|%15[^|]|%d|%d", kode, &paket, &dipakai) == 3 &&
        paket >= 0 && paket < jumlah_paket) {
        strcpy(daftar_token[jumlah_token].kode, kode);
        daftar_token[jumlah_token].paket = paket;
        daftar_token[jumlah_token].dipakai = dipakai ? 1 : 0;
        jumlah_token++;
    }
}

int muat_data(void) {
    FILE *f = fopen(FILE_DATA, "r");
    char line[256];
    int aktif = 0;
    time_t now = time(NULL);
    if (!f) return 0;

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strncmp(line, "PC|", 3) == 0)      muat_baris_pc(line, now, &aktif);
        else if (strncmp(line, "TK|", 3) == 0) muat_baris_token(line);
    }
    fclose(f);
    return aktif;
}

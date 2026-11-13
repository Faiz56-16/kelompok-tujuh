/* billing.c - Logika tagihan: biaya, struk, penghentian sesi, auto-lock. */
#include <stdio.h>
#include <time.h>
#include "config.h"
#include "state.h"
#include "util.h"
#include "storage.h"
#include "billing.h"

/* Billing per menit (dibulatkan ke atas, min. 1 menit), lalu dibulatkan
 * ke atas ke kelipatan Rp100. */
long hitung_biaya_postpaid(long detik) {
    long menit = (detik + 59) / 60;
    long biaya;
    if (menit < 1) menit = 1;
    biaya = (menit * TARIF_PER_JAM + 59) / 60;
    return ((biaya + 99) / 100) * 100;
}

static void cetak_struk(const komputer *k, long detik, long biaya, time_t selesai, int otomatis) {
    char mulai[32], akhir[32], durasi[16], rp[32];
    format_waktu(k->waktu_mulai, mulai, sizeof(mulai));
    format_waktu(selesai, akhir, sizeof(akhir));
    format_durasi(detik, durasi, sizeof(durasi));
    format_rupiah(biaya, rp, sizeof(rp));

    printf("\n");
    printf("  ==========================================\n");
    printf("              STRUK RENTAL WARNET\n");
    printf("  ==========================================\n");
    printf("  Unit       : PC-%02d\n", k->id);
    printf("  Pelanggan  : %s\n", k->nama_pelanggan);
    printf("  Mode       : %s\n", k->mode == MODE_PREPAID ? "Pre-Paid (Token)" : "Post-Paid (Reguler)");
    printf("  Mulai      : %s\n", mulai);
    printf("  Selesai    : %s\n", akhir);
    printf("  Durasi     : %s\n", durasi);
    if (k->mode == MODE_PREPAID) {
        char sisa[16];
        long s = k->durasi_paket - detik;
        format_durasi(s, sisa, sizeof(sisa));
        printf("  Paket      : %d menit\n", k->durasi_paket / 60);
        if (s > 0) printf("  Sisa waktu : %s (hangus)\n", sisa);
    } else {
        char tarif[32];
        format_rupiah(TARIF_PER_JAM, tarif, sizeof(tarif));
        printf("  Tarif      : %s/jam (per menit)\n", tarif);
    }
    printf("  ------------------------------------------\n");
    printf("  TOTAL      : %s\n", rp);
    printf("  ==========================================\n");
    if (otomatis)
        printf("  Waktu habis - PC-%02d dikunci otomatis.\n", k->id);
    else
        printf("  PC-%02d dikunci kembali. Terima kasih!\n", k->id);
}

void selesai_sesi(komputer *k, int otomatis) {
    time_t now = time(NULL);
    long detik = (long)difftime(now, k->waktu_mulai);
    long biaya;

    if (k->mode == MODE_PREPAID) {
        if (detik > k->durasi_paket) detik = k->durasi_paket;
        biaya = k->harga_paket;
    } else {
        biaya = hitung_biaya_postpaid(detik);
    }

    cetak_struk(k, detik, biaya, k->waktu_mulai + (time_t)detik, otomatis);

    k->terpakai = 0;
    k->nama_pelanggan[0] = '\0';
    k->waktu_mulai = 0;
    k->mode = MODE_POSTPAID;
    k->durasi_paket = 0;
    k->harga_paket = 0;
    k->peringatan = 0;
}

void cek_sesi(void) {
    time_t now = time(NULL);
    int i, berubah = 0;
    for (i = 0; i < JUMLAH_PC; i++) {
        komputer *k = &pc[i];
        long sisa;
        if (!k->terpakai || k->mode != MODE_PREPAID) continue;
        sisa = k->durasi_paket - (long)difftime(now, k->waktu_mulai);
        if (sisa <= 0) {
            printf("\n  *** [PC-%02d] WAKTU HABIS ***\n", k->id);
            selesai_sesi(k, 1);
            berubah = 1;
        } else if (sisa <= BATAS_PERINGATAN && !k->peringatan) {
            printf("\n  [PERINGATAN] PC-%02d (%s): sisa waktu %ld menit %ld detik!\n",
                   k->id, k->nama_pelanggan, sisa / 60, sisa % 60);
            k->peringatan = 1;
        }
    }
    if (berubah) simpan_data();
}

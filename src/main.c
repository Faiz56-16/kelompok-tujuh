/*
 * Program Rental Warnet Berbasis Console (Bahasa C)
 * Berdasarkan dokumentasi: https://github.com/Faiz56-16/sistem-login-c
 *
 * main.c hanya berisi inisialisasi dan loop menu; logika ada di modul lain.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "state.h"
#include "util.h"
#include "storage.h"
#include "billing.h"
#include "ui.h"
#include "rental.h"
#include "token.h"

int main(void) {
    int pilihan, lanjut;

    srand((unsigned)time(NULL));
    init_data();
    lanjut = muat_data();
    if (lanjut > 0)
        printf("\n[INFO] %d sesi aktif dilanjutkan dari data sebelumnya.\n", lanjut);

    do {
        cek_sesi();
        tampil_menu();
        pilihan = baca_int("Pilih menu [0-6]: ", 0, 6);
        if (input_habis()) break;
        cek_sesi();   /* waktu bisa habis saat menunggu input */

        switch (pilihan) {
            case 1: tampil_status(); tekan_enter(); break;
            case 2: sewa_postpaid(); tekan_enter(); break;
            case 3: stop_sewa();     tekan_enter(); break;
            case 4: buat_token();    tekan_enter(); break;
            case 5: login_token();   tekan_enter(); break;
            case 6: lihat_token();   tekan_enter(); break;
            case 0: break;
        }
    } while (pilihan != 0);

    simpan_data();
    printf("\nProgram selesai. Sesi yang masih aktif tersimpan di %s.\n", FILE_DATA);
    return 0;
}

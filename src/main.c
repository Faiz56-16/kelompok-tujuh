
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "state.h"
#include "util.h"
#include "storage.h"
#include "billing.h"
#include "ui.h"
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
        pilihan = baca_int("Pilih menu [0-5]: ", 0, 5);
        if (input_habis()) break;
        cek_sesi();   /* waktu bisa habis saat menunggu input */

        switch (pilihan) {
            case 1: tampil_status(); tekan_enter(); break;
            case 2: buat_token();    tekan_enter(); break;
            case 3: login_token();   tekan_enter(); break;
            case 4: lihat_token();   tekan_enter(); break;
            case 0: break;
        }
    } while (pilihan != 0);

    simpan_data();
    printf("\nProgram selesai. Sesi yang masih aktif tersimpan di %s.\n", FILE_DATA);
    return 0;
}

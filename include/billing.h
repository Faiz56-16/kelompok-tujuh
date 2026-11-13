/* billing.h - Hitung biaya, cetak struk, hentikan sesi, cek waktu habis. */
#ifndef BILLING_H
#define BILLING_H

#include "types.h"

long hitung_biaya_postpaid(long detik);
void selesai_sesi(komputer *k, int otomatis);   /* cetak struk + kosongkan PC */
void cek_sesi(void);                            /* peringatan 3 menit & auto-lock */

#endif

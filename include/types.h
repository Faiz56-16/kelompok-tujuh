/* types.h - Definisi struktur data utama. */
#ifndef TYPES_H
#define TYPES_H

#include <time.h>

typedef struct {
    int    id;                    /* Nomor/ID unit komputer */
    int    terpakai;              /* 0: Kosong, 1: Terpakai */
    char   nama_pelanggan[50];    /* Nama penyewa */
    time_t waktu_mulai;           /* Timestamp waktu mulai sewa (detik) */
    int    mode;                  /* MODE_POSTPAID / MODE_PREPAID */
    int    durasi_paket;          /* detik (khusus prepaid) */
    int    harga_paket;           /* rupiah (khusus prepaid) */
    int    peringatan;            /* 1 bila peringatan 3 menit sudah tampil */
} komputer;

typedef struct {
    const char *nama;
    int durasi;                   /* detik */
    int harga;                    /* rupiah */
} Paket;

typedef struct {
    char kode[16];
    int  paket;                   /* indeks paket_list */
    int  dipakai;                 /* 0: aktif, 1: sudah dipakai */
} Token;

#endif

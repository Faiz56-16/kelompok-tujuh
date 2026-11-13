# Dokumentasi Program Rental Warnet Berbasis Console (Bahasa C)

Aplikasi rental warnet sederhana ini beroperasi melalui antarmuka perintah/terminal (*console*). Sistem ini mengelola status komputer, mencatat waktu sewa secara otomatis menggunakan pustaka `time.h`, dan mengkalkulasi total biaya sewa saat transaksi dihentikan.

Selain sewa reguler (*post-paid*), program mendukung sewa paket berbasis **token** (*pre-paid*) dengan timer mundur, peringatan 3 menit terakhir, dan kunci otomatis saat waktu habis. Sesi disimpan ke file sehingga dapat dilanjutkan setelah program ditutup.

## Daftar Isi

1. [Fitur](#1-fitur)
2. [Build dan Menjalankan](#2-build-dan-menjalankan)
3. [Struktur Proyek](#3-struktur-proyek)
4. [Alur dan Algoritma Utama](#4-alur-dan-algoritma-utama)
5. [Struktur Data Utama](#5-struktur-data-utama)
6. [Konfigurasi](#6-konfigurasi)
7. [Dokumentasi Modul](#7-dokumentasi-modul)
8. [Perhitungan Biaya](#8-perhitungan-biaya)
9. [Sistem Token Pre-Paid](#9-sistem-token-pre-paid)
10. [Format File Penyimpanan](#10-format-file-penyimpanan)
11. [Contoh Tampilan](#11-contoh-tampilan)
12. [Panduan Modifikasi](#12-panduan-modifikasi)
13. [Batasan dan Catatan](#13-batasan-dan-catatan)

---

## 1. Fitur

- **5 unit komputer** (`PC-01` sampai `PC-05`) dengan status `KOSONG` atau `TERPAKAI`.
- **Post-Paid (Reguler)**: billing per menit, tarif Rp5.000/jam, total dihitung saat sewa dihentikan.
- **Pre-Paid (Token)**: kasir membuat token dari paket, pelanggan login memakai token, timer berjalan mundur.
- **Peringatan 3 menit terakhir** dan **kunci otomatis** (disertai struk) saat paket habis.
- **Struk tagihan** untuk kedua mode.
- **Penyimpanan sesi** ke file; saat program dibuka lagi, timer melanjutkan sisa waktu (waktu saat program mati tidak dihitung).
- **Kode modular**: setiap tanggung jawab dipisah ke modul tersendiri (`src/` dan `include/`).

---

## 2. Build dan Menjalankan

Dengan `make`:

```bash
make       # build the app
./warnet   # run program
make clean # clean workspace
```

Tanpa `make`:

```bash
gcc -O2 -Iinclude src/*.c -o warnet
```

> File data sesi (`sesi.dat`) dibuat di folder tempat program **dijalankan**, bukan di folder proyek.

---

## 3. Struktur Proyek

```
warnet/
├── Makefile
├── include/
│   ├── config.h    Konstanta (jumlah PC, tarif, batas peringatan, nama file data)
│   ├── types.h     Struct komputer, Paket, Token
│   ├── state.h     Data global (pc[], daftar_token[], paket_list[])
│   ├── util.h      Input aman + format durasi/rupiah/waktu
│   ├── storage.h   Simpan/muat sesi
│   ├── billing.h   Biaya, struk, stop sesi, auto-lock
│   ├── ui.h        Menu, tabel status, pilih unit, input nama
│   ├── rental.h    Sewa post-paid dan stop sewa
│   └── token.h     Buat/login/lihat token pre-paid
└── src/
    ├── main.c      Inisialisasi dan loop menu
    ├── state.c     Definisi data global dan daftar paket
    ├── util.c
    ├── storage.c
    ├── billing.c
    ├── ui.c
    ├── rental.c
    └── token.c
```

### Ketergantungan antar modul

```
main ──> rental, token, ui, billing, storage
rental / token ──> ui, billing, storage, state, util
ui      ──> billing (estimasi biaya), state, util
billing ──> storage, state, util
storage ──> state
state, util ──> (tidak bergantung ke modul lain)
```

---

## 4. Alur dan Algoritma Utama

1. **Inisialisasi Data** (`main.c`):
   - `srand()` diinisialisasi dengan waktu sekarang (untuk pembuatan token acak).
   - `init_data()` mengatur daftar PC (`PC-01` hingga `PC-05`) dengan status awal `KOSONG` (`terpakai = 0`) dan mengosongkan daftar token.
   - `muat_data()` membaca file data; jika ada sesi aktif, tampil pesan `[INFO] N sesi aktif dilanjutkan dari data sebelumnya.`
   - Tarif sewa per jam ditentukan di `config.h` (default: Rp5.000/jam).
2. **Main Menu (Perulangan Utama)**:
   - Di setiap putaran, `cek_sesi()` dipanggil **sebelum menu ditampilkan** dan **sekali lagi setelah pilihan dibaca** (waktu bisa habis saat program menunggu input).
   - Menampilkan opsi menu:
     1. Lihat Status Komputer (Billing Monitor)
     2. Sewa Komputer (Post-Paid / Reguler)
     3. Selesai / Cetak Struk (Stop Billing)
     4. Buat Token Paket (Kasir, Pre-Paid)
     5. Login dengan Token (Pre-Paid)
     6. Lihat Token Aktif
     0. Keluar
3. **Pemuatan Modul berdasarkan Input User**:
   - **Menu 1 (Status)**: Menampilkan tabel seluruh unit komputer beserta status, nama penyewa, mode, lama pemakaian, dan sisa waktu (pre-paid) atau estimasi biaya (post-paid).
   - **Menu 2 (Mulai Sewa)**: Memvalidasi pilihan unit PC yang masih `KOSONG`, meminta nama pelanggan, lalu menyimpan timestamp waktu mulai (`time(NULL)`) dengan mode post-paid.
   - **Menu 3 (Stop Sewa)**: Memvalidasi unit PC yang sedang `TERPAKAI`, mencatat timestamp waktu selesai, menghitung durasi sewa, dan mencetak struk tagihan. Berlaku untuk post-paid maupun pre-paid.
   - **Menu 4 (Buat Token)**: Memilih paket, program membuat kode token unik, kasir menerima pembayaran sesuai harga paket.
   - **Menu 5 (Login Token)**: Memvalidasi token (ada dan belum dipakai), memilih unit `KOSONG`, meminta nama, lalu memulai timer mundur sesuai durasi paket.
   - **Menu 6 (Lihat Token)**: Menampilkan token yang belum dipakai beserta paket dan harganya.
   - **Menu 0 (Keluar)**: Menghentikan perulangan; data disimpan terlebih dahulu.
4. **Setiap perubahan data** (mulai sewa, stop sewa, buat token, login token, kunci otomatis) langsung diikuti `simpan_data()`.

### Diagram alur singkat

```
        ┌──────────────┐
        │  init_data() │
        │  muat_data() │
        └──────┬───────┘
               ▼
        ┌──────────────┐◄──────────────────────┐
        │  cek_sesi()  │                       │
        │ tampil_menu()│                       │
        │  baca pilihan│                       │
        │  cek_sesi()  │                       │
        └──────┬───────┘                       │
               ▼                               │
   ┌───────────┴──────────────┐                │
   │ 1 status   4 buat token  │                │
   │ 2 sewa     5 login token ├────────────────┘
   │ 3 stop     6 lihat token │   (tekan ENTER, kembali ke menu)
   └───────────┬──────────────┘
               │ 0
               ▼
        simpan_data() → keluar
```

---

## 5. Struktur Data Utama

Didefinisikan di `include/types.h`.

### `komputer`

Struktur data `komputer` digunakan untuk mengelompokkan atribut setiap unit:

```c
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
```

| Field | Keterangan |
|---|---|
| `id` | Nomor unit (1 sampai `JUMLAH_PC`), ditampilkan sebagai `PC-01`, dst. |
| `terpakai` | `0` kosong, `1` sedang dipakai |
| `nama_pelanggan` | Maksimal 49 karakter; karakter `\|` diganti spasi agar aman untuk file data |
| `waktu_mulai` | Timestamp mulai sesi; lama sesi = `now - waktu_mulai` |
| `mode` | `MODE_POSTPAID` (0) atau `MODE_PREPAID` (1) |
| `durasi_paket` | Durasi paket dalam detik (hanya pre-paid) |
| `harga_paket` | Harga paket dalam rupiah (hanya pre-paid) |
| `peringatan` | Penanda agar peringatan 3 menit hanya tampil sekali per sesi |

### `Paket`

```c
typedef struct {
    const char *nama;
    int durasi;                   /* detik */
    int harga;                    /* rupiah */
} Paket;
```

### `Token`

```c
typedef struct {
    char kode[16];
    int  paket;                   /* indeks paket_list */
    int  dipakai;                 /* 0: aktif, 1: sudah dipakai */
} Token;
```

### Data global (`state.h` / `state.c`)

| Variabel | Tipe | Keterangan |
|---|---|---|
| `pc[JUMLAH_PC]` | `komputer[]` | Seluruh unit komputer |
| `daftar_token[MAKS_TOKEN]` | `Token[]` | Token yang pernah dibuat |
| `jumlah_token` | `int` | Jumlah token terisi |
| `paket_list[]` | `const Paket[]` | Daftar paket pre-paid |
| `jumlah_paket` | `const int` | Banyak paket (dihitung otomatis dengan `sizeof`) |

Paket bawaan:

| No | Nama | Durasi | Harga |
|---|---|---|---|
| 1 | Paket test 1 Menit | 60 detik | Rp100 |
| 2 | Paket 1 Jam | 3600 detik | Rp5.000 |
| 3 | Paket 2 Jam | 7200 detik | Rp9.000 |
| 4 | Paket 3 Jam | 10800 detik | Rp13.000 |

---

## 6. Konfigurasi

Semua konstanta ada di `include/config.h`:

| Konstanta | Nilai | Keterangan |
|---|---|---|
| `JUMLAH_PC` | `5` | Jumlah unit komputer |
| `TARIF_PER_JAM` | `5000` | Tarif post-paid, rupiah per jam |
| `MAKS_TOKEN` | `100` | Kapasitas daftar token |
| `BATAS_PERINGATAN` | `180` | Batas peringatan (detik) = 3 menit |
| `FILE_DATA` | `"sesi.dat"` | Nama file penyimpanan |
| `MODE_POSTPAID` | `0` | Penanda mode reguler |
| `MODE_PREPAID` | `1` | Penanda mode token |

---

## 7. Dokumentasi Modul

### 7.1 `main.c`

Hanya berisi inisialisasi dan loop menu; seluruh logika ada di modul lain. Pilihan menu dibaca dengan `baca_int("Pilih menu [0-6]: ", 0, 6)`. Jika input habis (EOF), loop dihentikan. Setelah setiap aksi menu, program menunggu `ENTER` (`tekan_enter()`).

### 7.2 `util` (input dan format)

Tidak berisi logika bisnis.

| Fungsi | Keterangan |
|---|---|
| `baca_baris(prompt, buf, n)` | Membaca satu baris dengan `fgets`; sisa baris yang terlalu panjang dibuang; menandai EOF |
| `baca_int(prompt, min, max)` | Mengulang sampai angka valid pada rentang `[min, max]`; mengembalikan `min-1` bila EOF |
| `tekan_enter()` | Jeda sampai pengguna menekan ENTER |
| `input_habis()` | `1` bila `stdin` sudah EOF |
| `format_durasi(detik, out, sz)` | Detik menjadi `HH:MM:SS` (nilai negatif dianggap 0) |
| `format_rupiah(n, out, sz)` | Angka menjadi `Rp12.500` (pemisah ribuan titik) |
| `format_waktu(t, out, sz)` | Timestamp menjadi `dd-mm-yyyy HH:MM:SS` |

### 7.3 `state` (data global)

| Fungsi | Keterangan |
|---|---|
| `init_data()` | Mengosongkan `pc[]` dan `daftar_token[]`, mengisi `id` setiap PC (1..`JUMLAH_PC`), mengatur `jumlah_token = 0` |

### 7.4 `storage` (penyimpanan)

| Fungsi | Keterangan |
|---|---|
| `simpan_data()` | Menulis seluruh PC dan token ke `FILE_DATA`; yang disimpan adalah **detik terpakai**, bukan timestamp |
| `muat_data()` | Membaca file; mengembalikan jumlah sesi aktif yang dilanjutkan. Fungsi internal: `muat_baris_pc()`, `muat_baris_token()` |

Saat dimuat, `waktu_mulai` dihitung ulang sebagai `now - detik_terpakai`, sehingga timer melanjutkan sisa waktu tanpa menghitung waktu saat program mati. Detail format ada di [bagian 10](#10-format-file-penyimpanan).

### 7.5 `billing` (tagihan)

| Fungsi | Keterangan |
|---|---|
| `hitung_biaya_postpaid(detik)` | Menghitung biaya reguler (lihat [bagian 8](#8-perhitungan-biaya)) |
| `selesai_sesi(k, otomatis)` | Menghitung durasi dan biaya, mencetak struk, lalu mengosongkan PC. Parameter `otomatis = 1` bila dipicu waktu habis |
| `cek_sesi()` | Untuk setiap PC pre-paid yang terpakai: kunci otomatis bila sisa waktu `<= 0`, atau tampilkan peringatan sekali bila sisa `<= BATAS_PERINGATAN`. Menyimpan data bila ada PC yang dikunci |
| `cetak_struk(...)` | (internal, `static`) Mencetak struk |

Aturan `selesai_sesi()`:

- **Post-paid**: durasi = `now - waktu_mulai`, biaya dari `hitung_biaya_postpaid()`.
- **Pre-paid**: durasi dibatasi maksimal `durasi_paket`, biaya = `harga_paket`. Bila dihentikan lebih awal, sisa waktu **hangus** (tidak ada pengembalian) dan dicantumkan di struk.

### 7.6 `ui` (tampilan)

| Fungsi | Keterangan |
|---|---|
| `tampil_menu()` | Menampilkan menu utama beserta tarif reguler |
| `tampil_status()` | Menampilkan tabel status semua PC (kolom: Unit, Status, Pelanggan, Mode, Terpakai, Sisa/Estimasi) |
| `pilih_unit(want_terpakai)` | Menampilkan daftar unit `KOSONG` (0) atau `TERPAKAI` (1), meminta nomor PC; mengembalikan indeks PC atau `-1` bila batal/tidak ada |
| `baca_nama(k)` | Meminta nama pelanggan (tidak boleh kosong), mengganti `\|` dengan spasi |

### 7.7 `rental` (sewa reguler)

| Fungsi | Keterangan |
|---|---|
| `sewa_postpaid()` | Memilih unit `KOSONG`, membaca nama, mengatur mode post-paid dan `waktu_mulai = time(NULL)`, menyimpan data |
| `stop_sewa()` | Memilih unit `TERPAKAI`, memanggil `selesai_sesi(..., 0)`, menyimpan data |

### 7.8 `token` (pre-paid)

| Fungsi | Keterangan |
|---|---|
| `buat_token()` | Memilih paket, membuat kode unik, menyimpan token, menampilkan jumlah yang harus dibayar |
| `login_token()` | Memvalidasi token, memilih unit `KOSONG`, membaca nama, memulai timer paket |
| `lihat_token()` | Menampilkan token yang belum dipakai |
| `tampil_paket()`, `bersihkan_token_terpakai()`, `token_ada()` | (internal, `static`) |

Bila daftar token penuh (`MAKS_TOKEN`), token yang sudah dipakai dibersihkan otomatis terlebih dahulu.

---

## 8. Perhitungan Biaya

Berlaku untuk mode **post-paid**. Billing dihitung per menit, dibulatkan ke atas, minimal 1 menit:

```
menit = ceil(detik / 60)            (minimal 1)
biaya = ceil(menit * TARIF_PER_JAM / 60)
biaya = dibulatkan ke atas ke kelipatan Rp100
```

Implementasi:

```c
long hitung_biaya_postpaid(long detik) {
    long menit = (detik + 59) / 60;
    long biaya;
    if (menit < 1) menit = 1;
    biaya = (menit * TARIF_PER_JAM + 59) / 60;
    return ((biaya + 99) / 100) * 100;
}
```

Contoh dengan tarif Rp5.000/jam:

| Durasi pemakaian | Menit ditagih | Hitungan | Biaya |
|---|---|---|---|
| 10 detik | 1 | 5000/60 = 83,3 → 84 → 100 | Rp100 |
| 90 detik | 2 | 10000/60 = 166,7 → 167 → 200 | Rp200 |
| 30 menit | 30 | 2500 | Rp2.500 |
| 12 menit 30 detik | 13 | 1083,3 → 1084 → 1100 | Rp1.100 |
| 1 jam | 60 | 5000 | Rp5.000 |
| 61 menit | 61 | 5083,3 → 5084 → 5100 | Rp5.100 |

Mode **pre-paid** tidak dihitung per menit: biaya selalu sama dengan harga paket.

---

## 9. Sistem Token Pre-Paid

**Alur:**

1. **Kasir** memilih menu 4, memilih paket, dan menerima pembayaran sesuai harga. Program menampilkan kode token.
2. **Pelanggan** memberikan kode di menu 5, memilih unit `KOSONG`, dan mengisi nama.
3. Timer mundur dimulai. Token otomatis ditandai `dipakai` (tidak bisa dipakai ulang).
4. Saat sisa waktu `<= 3 menit`, muncul peringatan sekali.
5. Saat waktu habis, PC dikunci otomatis dan struk dicetak.

**Format kode token:** `WN-` diikuti 6 karakter acak, misalnya `WN-A6VNG7`. Karakter diambil dari:

```
ABCDEFGHJKLMNPQRSTUVWXYZ23456789
```

Huruf `I`, `O` dan angka `0`, `1` sengaja dibuang agar tidak mudah tertukar. Kode dijamin unik terhadap token yang ada. Input kode saat login tidak membedakan huruf besar/kecil.

**Pesan kesalahan login:**

| Pesan | Penyebab |
|---|---|
| `[X] Token tidak ditemukan.` | Kode tidak ada di daftar |
| `[X] Token sudah pernah dipakai.` | Token sudah digunakan sebelumnya |
| `Login dibatalkan (token belum terpakai).` | Pengguna membatalkan pemilihan unit; token tetap aktif |

---

## 10. Format File Penyimpanan

File `sesi.dat` berupa teks, satu record per baris, dipisah tanda `|`.

**Baris PC**

```
PC|id|terpakai|mode|detik_terpakai|durasi_paket|harga_paket|nama
```

**Baris token**

```
TK|kode|paket|dipakai
```

**Contoh isi file:**

```
PC|1|1|1|23|60|100|may
PC|2|1|1|44|60|100|pega
PC|3|1|1|12|60|100|paiza
PC|4|1|0|52|60|100|jaki
PC|5|0|0|0|0|0|
TK|WN-WKRP2K|0|1
```

**Cara membaca baris `PC|1|1|1|23|60|100|jaki`:**

| Bagian | Nilai | Artinya |
|---|---|---|
| `PC` | | Penanda baris data komputer |
| id | `1` | PC-01 |
| terpakai | `1` | 1 = dipakai, 0 = kosong |
| mode | `1` | 0 = post-paid, 1 = pre-paid |
| detik_terpakai | `23` | Sesi sudah berjalan 23 detik saat disimpan |
| durasi_paket | `60` | Paket 60 detik (1 menit) |
| harga_paket | `100` | Rp100 |
| nama | `may` | Nama pelanggan |

**Cara membaca baris `TK|WN-WKRP2K|0|1`:**

| Bagian | Nilai | Artinya |
|---|---|---|
| kode | `WN-WKRP2K` | Kode token |
| paket | `0` | Indeks `paket_list` (0 = paket pertama) |
| dipakai | `1` | 1 = sudah dipakai, 0 = masih aktif |

Baris PC yang kosong tetap ditulis dengan semua angka `0` dan nama kosong.

**Mengapa menyimpan detik terpakai, bukan jam mulai?** Saat dimuat, program menghitung `waktu_mulai = sekarang - detik_terpakai`. Dengan begitu timer melanjutkan dari titik terakhir dan waktu saat program mati tidak ikut dihitung.

---

## 11. Contoh Tampilan

### Menu utama

```
==============================================
        RENTAL WARNET - MENU UTAMA
        Tarif reguler: Rp5.000/jam
==============================================
  1. Lihat Status komputer (Billing Monitor)
  2. Sewa komputer (Post-Paid / Reguler)
  3. Selesai / Cetak Struk (Stop Billing)
  4. Buat Token Paket (Kasir, Pre-Paid)
  5. Login dengan Token (Pre-Paid)
  6. Lihat Token Aktif
  0. Keluar
----------------------------------------------
Pilih menu [0-6]:
```

### Tabel status

```
  +-------+----------+--------------------+-----------+------------+----------------+
  | Unit  | Status   | Pelanggan          | Mode      | Terpakai   | Sisa / Estimasi|
  +-------+----------+--------------------+-----------+------------+----------------+
  | PC-01 | TERPAKAI | Siti               | Pre-Paid  | 00:00:23   | Sisa 00:00:37  |
  | PC-02 | TERPAKAI | Budi Santoso       | Post-Paid | 00:12:30   | Est. Rp1.100   |
  | PC-03 | KOSONG   | -                  | -         | -          | -              |
  | PC-04 | KOSONG   | -                  | -         | -          | -              |
  | PC-05 | KOSONG   | -                  | -         | -          | -              |
  +-------+----------+--------------------+-----------+------------+----------------+
```

### Struk post-paid

```
  ==========================================
              STRUK RENTAL WARNET
  ==========================================
  Unit       : PC-02
  Pelanggan  : pega
  Mode       : Post-Paid (Reguler)
  Mulai      : 14-11-2026 10:00:00
  Selesai    : 14-11-2026 10:12:30
  Durasi     : 00:12:30
  Tarif      : Rp5.000/jam (per menit)
  ------------------------------------------
  TOTAL      : Rp1.100
  ==========================================
  PC-02 dikunci kembali. Terima kasih!
```

### Peringatan dan kunci otomatis (pre-paid)

```
  [PERINGATAN] PC-01 (Siti): sisa waktu 1 menit 0 detik!

  *** [PC-01] WAKTU HABIS ***
  ... (struk) ...
  Waktu habis - PC-01 dikunci otomatis.
```

---

## 12. Panduan Modifikasi

| Ingin mengubah... | Edit file |
|---|---|
| Tarif per jam, jumlah PC, batas peringatan | `include/config.h` |
| Nama file data | `include/config.h` (`FILE_DATA`) |
| Daftar/harga paket pre-paid | `src/state.c` (`paket_list`) |
| Rumus biaya / format struk | `src/billing.c` |
| Tampilan menu dan tabel | `src/ui.c` |
| Format file penyimpanan | `src/storage.c` |
| Menambah menu baru | modul baru + `main.c` (`switch`) |

**Menambah paket baru:** tambahkan satu baris di `paket_list` pada `src/state.c`. Jumlah paket dihitung otomatis lewat `sizeof`, dan menu pembuatan token ikut menyesuaikan.

```c
const Paket paket_list[] = {
    { "Paket test 1 Menit",      60,    100 },
    { "Paket 1 Jam",           3600,   5000 },
    { "Paket 2 Jam",           7200,   9000 },
    { "Paket 3 Jam",          10800,  13000 },
    { "Paket 5 Jam",          18000,  20000 },   /* contoh paket baru */
};
```

> Urutan paket memengaruhi data lama: `TK|...|paket|...` menyimpan **indeks** paket. Tambahkan paket baru di akhir daftar, jangan menyisipkan di tengah atau mengubah urutan bila sudah ada token tersimpan.

**Menambah menu baru:**

1. Buat fungsi di modul yang sesuai (atau modul baru) beserta deklarasinya di header.
2. Tambahkan baris di `tampil_menu()` (`src/ui.c`).
3. Perbesar rentang `baca_int(..., 0, N)` dan tambahkan `case` di `main.c`.

**Mengubah jumlah PC:** ubah `JUMLAH_PC` di `config.h`. Lebar tabel status tidak terpengaruh, tetapi file data lama dengan jumlah PC berbeda akan dibaca sebagian (baris PC dengan `id` di luar rentang diabaikan).

---

## 13. Batasan dan Catatan

- **Pengecekan waktu habis** dilakukan setiap kali kembali ke menu dan setelah memilih menu, bukan terus-menerus di latar belakang. Penyebabnya, input console bersifat memblokir. Kunci otomatis baru terlihat setelah pengguna menekan ENTER atau memilih menu.
- **Sesi yang dilanjutkan**: waktu saat program mati tidak dihitung. Flag peringatan 3 menit tidak disimpan, sehingga peringatan dapat tampil lagi setelah program dibuka kembali.
- **Satu terminal** berperan sebagai kasir sekaligus "PC client". Program ini tidak memiliki komunikasi jaringan antar komputer.
- **Nama pelanggan** maksimal 49 karakter, karakter `|` diganti spasi.
- **Kapasitas token** `MAKS_TOKEN` = 100; token terpakai dibersihkan otomatis saat penuh.
- **Pre-paid dihentikan lebih awal**: sisa waktu hangus dan tidak ada pengembalian dana.
- **Paket uji** "Paket test 1 Menit" (Rp100) disediakan untuk menguji timer tanpa menunggu lama. Hapus baris tersebut di `src/state.c` untuk penggunaan nyata.
- **Input** divalidasi: angka di luar rentang atau teks non-angka akan diminta ulang; jika `stdin` ditutup (EOF) program keluar dengan menyimpan data.
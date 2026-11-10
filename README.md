# Dokumentasi Program Rental Warnet Berbasis Console (Bahasa C)

Aplikasi rental warnet sederhana ini beroperasi melalui antarmuka perintah/terminal (*console*). Sistem ini mengelola status komputer, mencatat waktu sewa secara otomatis menggunakan pustaka `time.h`, dan mengkalkulasi total biaya sewa saat transaksi dihentikan.

---

## 1. Alur & Algoritma Utama

1. **Inisialisasi Data**:
   - Mengatur daftar PC (`PC-01` hingga `PC-05`) dengan status awal `KOSONG` (`terpakai = 0`).
   - Menentukan tarif sewa per jam (misal: Rp5.000/jam).
2. **Main Menu (Perulangan Utama)**:
   - Menampilkan opsi menu:
     1. Lihat Status Komputer (Billing Monitor)
     2. Sewa Komputer (Mulai Billing)
     3. Selesai / Cetak Struk (Stop Billing)
     4. Keluar
3. **Pemuatan Modul berdasarkan Input User**:
   - **Menu 1 (Status)**: Menampilkan tabel seluruh unit komputer beserta status dan nama penyewa.
   - **Menu 2 (Mulai Sewa)**: Memvalidasi pilihan unit PC yang masih `KOSONG`, meminta nama pelanggan, lalu menyimpan timestamp waktu mulai (`time(NULL)`).
   - **Menu 3 (Stop Sewa)**: Memvalidasi unit PC yang sedang `TERPAKAI`, mencatat timestamp waktu selesai, menghitung durasi sewa, dan mencetak struk tagihan.
   - **Menu 4 (Keluar)**: Menghentikan perulangan program.

---

## 2. Struktur Data Utama

Struktur data `Komputer` digunakan untuk mengelompokkan atribut setiap unit:

```c
typedef struct {
    int id;                  // Nomor/ID unit komputer
    int terpakai;            // 0: Kosong, 1: Terpakai
    char nama_pelanggan[50]; // Nama penyewa
    time_t waktu_mulai;      // Timestamp waktu mulai sewa (detik)
} Komputer;


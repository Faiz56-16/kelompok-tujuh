# Panduan & Alur Kerja Sistem Manajemen Warnet

Dokumen ini menjelaskan alur kerja operasional, logika bisnis, dan cara kerja sistem billing manajemen warnet secara konseptual tanpa kode program.

---

## 1. Komponen Utama Sistem

Sistem manajemen warnet terbagi menjadi dua aplikasi utama yang saling terhubung dalam satu jaringan LAN:

1. **Aplikasi Server (Kasir / Admin)**
   * Berada di komputer kasir.
   * Berfungsi memantau status seluruh PC client, mengelola transaksi pembayaran, membuat token/voucher, dan mengirim perintah (unlock/lock/message) ke PC client.

2. **Aplikasi Client (Komputer Pengguna)**
   * Berjalan di setiap unit PC warnet.
   * Berfungsi mengunci layar (*Screen Lock*) saat PC kosong, memverifikasi login pengguna, dan menampilkan *floating timer* (penghitung waktu sisa) saat PC digunakan.

---

## 2. Alur Operasional Pelanggan

```text
               [ Pelanggan Datang ]
                        │
                        ▼
            ┌──────────────────────┐
            │ Pilih Mode Pembayaran│
            └───────────┬──────────┘
                        │
       ┌────────────────┴────────────────┐
       ▼                                 ▼
[ Mode Pre-Paid / Paket ]      [ Mode Post-Paid / Reguler ]
       │                                 │
  1. Bayar ke kasir               1. Kasir unlock PC dari Server
  2. Diberi Token / Akun          2. Pelanggan langsung pakai PC
  3. Input token di PC Client     3. Timer menghitung dari nol
  4. Timer mundur berjalan        4. Billing dihitung per menit
       │                                 │
       ▼                                 ▼
[ Waktu Habis ]                 [ Pelanggan Selesai ]
  • Warning 3 menit              • Klik tombol Selesai / Ke Kasir
  • Layar otomatis terkunci      • Kasir kunci PC dari Server
  • Aplikasi ditutup             • Kasir cetak tagihan & bayar
```

---

## 3. Tahapan Cara Kerja Sistem

### Tahap 1: Kondisi Awal (Standby)
* Seluruh komputer client berada dalam keadaan terkunci (*Screen Lock*).
* Pengguna tidak bisa mengakses desktop, taskbar, atau membuka aplikasi sebelum melakukan autentikasi login atau di-unlock oleh kasir.

### Tahap 2: Proses Login & Pembukaan Akses
* **Mode Pre-Paid (Paket / Token):**
  * Pelanggan membayar sejumlah uang di kasir (misal: Paket 2 Jam).
  * Kasir membuat kode voucher/token.
  * Pelanggan memasukkan kode voucher pada layar terkunci PC Client.
  * PC Client mengirimkan kode ke Server untuk diverifikasi. Jika sah, layar terbuka dan timer mundur dimulai.
* **Mode Post-Paid (Bayar Belakangan):**
  * Pelanggan langsung memilih nomor PC yang kosong.
  * Kasir melakukan perintah *Unlock* PC dari aplikasi Server.
  * PC Client terbuka dan mulai menghitung durasi pemakaian beserta estimasi biaya secara berjalan (*real-time*).

### Tahap 3: Sesi Pemakaian & Sinkronisasi (Heartbeat)
* Selama PC digunakan, aplikasi Client mengirimkan sinyal "Heartbeat" (tanda aktif) ke Server setiap beberapa detik.
* Server memperbarui status PC pada layar monitor kasir (misal: "PC-05: Aktif, Sisa Waktu 45 Menit").
* Widget floating timer kecil akan selalu tampil di sudut layar PC Client agar pengguna tahu sisa waktu mereka.

### Tahap 4: Sistem Peringatan & Penguncian Otomatis
* **Peringatan (Warning):** Ketika sisa waktu paket menyisakan 3 menit, sistem akan menampilkan notifikasi *pop-up* peringatan kepada pelanggan.
* **Selesai Waktu (Prepaid):** 
  * Saat timer mencapai `00:00:00`, sistem secara otomatis mengunci kembali layar PC Client.
  * Aplikasi yang berjalan akan ditutup (*force logout*) demi menjaga privasi data pengguna berikutnya.
* **Selesai Pemakaian (Postpaid):**
  * Pelanggan menekan tombol "Selesai" atau melapor ke kasir.
  * Kasir menghentikan sesi dari Server.
  * Sistem menghitung total biaya berdasarkan durasi pemakaian dan mengunci kembali PC Client.

---

## 4. Keamanan & Penanganan Masalah (Error Handling)

* **Koneksi Terputus (Network Disconnect):** Jika kabel LAN terlepas atau koneksi ke Server terputus lebih dari 30 detik, PC Client akan masuk ke mode amannya dengan mengunci layar untuk mencegah pemakaian ilegal.
* **Mati Lampu / Restart Paksa:** Server menyimpan riwayat sesi secara berkala. Jika PC Client melakukan *restart* di tengah sesi yang aktif, saat menyala kembali sistem akan otomatis melanjutkan sesi timer yang tersisa tanpa merugi pengguna.
# Dokumentasi Workflow & Spesifikasi Sistem Manajemen W

Dokumen ini berisi spesifikasi perancangan sistem manajemen internet cafe (warnet), mencakup diagram alur (*workflow*), aturan bisnis (*business rules*), arsitektur sistem, struktur data C, dan protokol komunikasi Client-Server.

---

## 1. Flowchart & Alur Operasional (User & Admin Flow)

```text
[ Pelanggan Datang ]
       │
       ▼
┌────────────────────────┐
│ Pilih Mode Pembayaran  │
└───────────┬────────────┘
            ├──────────────────────────┐
            ▼                          ▼
    [ Mode Pre-Paid ]          [ Mode Post-Paid ]
(Bayar paket jam di awal)   (Billing berjalan sesuai durasi)
            │                          │
            ▼                          ▼
┌────────────────────────┐  ┌────────────────────────┐
│ Kasir Generate Token / │  │ Kasir Unlock PC Client │
│ Input Saldo Member     │  │ via Admin Server       │
└───────────┬────────────┘  └───────────┬────────────┘
            │                          │
            └───────────┬──────────────┘
                        ▼
            ┌────────────────────────┐
            │ User Login di PC Client│
            └───────────┬────────────┘
                        │
                        ▼
            ┌────────────────────────┐
            │ Session Timer Berjalan │
            │ (Sync ke Server)       │
            └───────────┬────────────┘
                        │
       ┌────────────────┴────────────────┐
       ▼                                 ▼
[ Waktu Habis (Prepaid) ]     [ User Checkout (Postpaid) ]
       │                                 │
       ▼                                 ▼
┌────────────────────────┐    ┌────────────────────────┐
│ PC Ter-lock Otomatis   │    │ Hitung Total Durasi &  │
│ (Layar Kunci Kembali)  │    │ Tagihan di Kasir       │
└────────────────────────┘    └───────────┬────────────┘
                                          │
                                          ▼
                              ┌────────────────────────┐
                              │ Transaksi Selesai &    │
                              │ PC Ter-lock Otomatis   │
                              └────────────────────────┘
```

---

## 2. Logika Bisnis & Aturan Operasional

### 2.1 Access Control PC Client
* **Default State:** Seluruh unit PC Client berada dalam status *Screen Locked* (layar terkunci penuh) sebelum sesi dimulai.
* **Metode Login:**
  1. **Kode Voucher / Token:** Kode unik sekali pakai untuk durasi tertentu.
  2. **Akun Member:** Login menggunakan `username` dan `password` dengan sistem potong saldo/waktu akun.
  3. **Direct Unlock (Kasir):** Kasir membuka akses PC dari panel Server (biasanya untuk mode *Post-paid*).

### 2.2 Perhitungan Tarif (Billing Engine)
* **Mode Post-paid (Reguler):**
  $$\text{Total Biaya} = \left\lceil \frac{\text{Durasi (detik)}}{60} \right\rceil \times \text{Tarif Per Menit}$$
* **Mode Pre-paid (Paket):**
  $$\text{Sisa Waktu} = \text{Durasi Paket (detik)} - (\text{Waktu Sekarang} - \text{Waktu Mulai})$$

### 2.3 Heartbeat & Synchronization
* PC Client wajib mengirimkan paket **Heartbeat** ke Server setiap 5 detik.
* Jika Server tidak menerima Heartbeat dari Client selama lebih dari 30 detik (misal: kabel LAN terlepas), status PC di Server berubah menjadi `DISCONNECTED`.

---

## 3. Struktur Data Utama (Bahasa C)

Berikut adalah definisi struktur data C untuk modul manajemen sesi billing warnet:

```c
#ifndef WARNET_SYSTEM_H
#define WARNET_SYSTEM_H

#include <stdio.h>
#include <time.h>

// Status Operasional PC Client
typedef enum {
    STATUS_AVAILABLE = 0,
    STATUS_IN_USE,
    STATUS_MAINTENANCE,
    STATUS_DISCONNECTED
} PCStatus;

// Mode Billing Pembayaran
typedef enum {
    MODE_PREPAID = 0,
    MODE_POSTPAID
} BillingType;

// Struktur Data Sesi PC Client
typedef struct {
    int pc_id;                  // Nomor ID Komputer (misal: 1 - 20)
    char username[32];          // Nama User atau Kode Voucher
    PCStatus status;            // Status PC saat ini
    BillingType billing_type;   // Mode Pembayaran (Prepaid / Postpaid)
    time_t start_time;          // Unix timestamp saat sesi dimulai
    int duration_allocated_sec; // Total durasi paket dalam detik (Prepaid)
    double rate_per_hour;       // Tarif per jam
    double current_cost;        // Biaya berjalan (Postpaid)
} PCSession;

// Struktur Data Informasi Kasir / Transaction Log
typedef struct {
    int transaction_id;
    int pc_id;
    char customer_name[32];
    BillingType billing_type;
    int total_minutes;
    double total_amount;
    time_t timestamp;
} TransactionRecord;

#endif // WARNET_SYSTEM_H
```

---

## 4. Matriks Respon Server & Event Handling

| Event / Trigger | Aksi Server | Respon / Instruksi ke Client |
| :--- | :--- | :--- |
| **Login Request Valid** | Buat sesi baru, ubah status PC ke `STATUS_IN_USE`. | Send `CMD_UNLOCK_SCREEN` & tampilkan widget timer floating. |
| **Login Request Invalid** | Catat log kegagalan login. | Send `CMD_AUTH_FAILED` (Tampilkan pesan error di client). |
| **Sisa Waktu $\le 3$ Menit** | Pemicu peringatan otomatis. | Send `CMD_SHOW_WARNING` ("Waktu Anda tersisa 3 menit lagi"). |
| **Sisa Waktu $= 0$ (Prepaid)**| Tutup sesi, simpan log transaksi, set status `STATUS_AVAILABLE`. | Send `CMD_LOCK_SCREEN` & kill aplikasi user. |
| **User Request Checkout** | Hitung biaya akhir (Postpaid), update laporan pendapatan. | Send `CMD_LOCK_SCREEN` & tampilkan rincian tagihan akhir. |
| **Heartbeat Timeout** | Flag PC sebagai `STATUS_DISCONNECTED`. | (Offline Mode) Client mengunci diri setelah *timeout* internal. |

---

## 5. Protocol Protocol Message Payload (TCP Socket JSON Concept)

### Client $\rightarrow$ Server: Heartbeat Ping
```json
{
  "event": "HEARTBEAT",
  "pc_id": 5,
  "username": "user_pro123",
  "uptime_sec": 1200
}
```

### Server $\rightarrow$ Client: Lock Command (Waktu Habis)
```json
{
  "command": "LOCK_CLIENT",
  "pc_id": 5,
  "reason": "SESSION_EXPIRED",
  "message": "Waktu billing Anda telah habis. Terima kasih!"
}
```
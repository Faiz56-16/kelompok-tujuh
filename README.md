Account Management System

1. Deskripsi Program

Account Management System merupakan aplikasi berbasis bahasa C yang berjalan melalui terminal (Console-Based Application). Program ini menyediakan sistem pengelolaan akun sederhana yang memungkinkan pengguna membuat akun, melakukan autentikasi login, dan mereset password menggunakan password hint.

2. Main Menu

Ketika program dijalankan, sistem menampilkan menu utama:

================================
       ACCOUNT MANAGEMENT
================================
[1] Buat Akun
[2] Login Akun
[3] Lupa Password
[4] Keluar Program
================================
Pilih menu:

Pengguna dapat memilih salah satu dari empat opsi yang tersedia.

3. Workflow

A. Buat Akun (Register)

1. Pengguna memilih menu "Buat Akun".
2. Sistem meminta informasi berikut:
   - Nama lengkap
   - Username
   - Password
   - Password Hint
3. Sistem melakukan validasi username untuk memastikan username belum digunakan.
4. Jika username sudah terdaftar, proses registrasi dibatalkan.
5. Jika username tersedia, data akun disimpan ke database lokal ("accounts.txt").
6. Sistem menampilkan notifikasi bahwa akun berhasil dibuat.
7. Pengguna dikembalikan ke menu utama.

B. Login Akun (Authentication)

1. Pengguna memilih menu "Login Akun".
2. Sistem meminta:
   - Username
   - Password
3. Sistem mencari username di database.
4. Jika username tidak ditemukan, sistem menampilkan notifikasi bahwa akun tidak tersedia.
5. Jika username ditemukan, sistem memverifikasi password.
6. Jika password benar, sistem menampilkan pesan login berhasil beserta nama pengguna.
7. Jika password salah, sistem menampilkan pesan login gagal.
8. Pengguna dikembalikan ke menu utama.

C. Lupa Password (Password Recovery)

1. Pengguna memilih menu "Lupa Password".
2. Sistem meminta username.
3. Sistem mencari username di database.
4. Jika username tidak ditemukan, sistem menampilkan notifikasi bahwa akun tidak tersedia.
5. Jika username ditemukan, sistem meminta Password Hint.
6. Sistem mencocokkan Password Hint dengan data yang tersimpan.
7. Jika Password Hint salah, proses reset dibatalkan.
8. Jika Password Hint benar, sistem memberikan akses untuk mengganti password.
9. Pengguna memasukkan password baru.
10. Sistem memperbarui password pada database lokal.
11. Sistem menampilkan notifikasi bahwa password berhasil diubah.
12. Pengguna dikembalikan ke menu utama.

D. Keluar Program (Exit)

1. Pengguna memilih menu "Keluar Program".
2. Sistem menghentikan perulangan menu utama.
3. Program menampilkan pesan penutup.
4. Program berakhir.

4. Struktur Data Akun

Setiap akun memiliki empat atribut:

Atribut| Deskripsi
"name"| Nama lengkap pengguna
"username"| Identitas unik untuk login
"password"| Kredensial autentikasi
"hint"| Informasi verifikasi untuk reset password

5. Struktur Modular

File| Tanggung Jawab
"main.c"| Menu utama dan navigasi program
"account.h"| Deklarasi struktur data dan fungsi
"account.c"| Logika registrasi, login, dan reset password
"storage.c"| Pengelolaan penyimpanan dan pembacaan database
"Makefile"| Otomatisasi proses kompilasi
"accounts.txt"| Penyimpanan data akun

6. Alur Sistem

           START
             |
             v
        MAIN MENU
             |
      +------+------+------+
      |      |      |      |
      v      v      v      v
   REGISTER LOGIN  RESET  EXIT
      |      |      |      |
      v      v      v      v
   VALIDASI AUTH  VERIFY  END
   USERNAME PASSWORD  HINT
      |      |      |
      v      v      v
   SAVE DB  LOGIN  NEW PASSWORD
      |    RESULT    |
      |      |       v
      |      |    UPDATE DB
      |      |       |
      +------+------+ 
             |
             v
         MAIN MENU

7. Teknologi dan Materi

- Programming Language: C
- Application Type: Console-Based Application (CLI)
- Compiler: GCC
- Build System: GNU Make
- Data Storage: Local Text File
- Programming Concepts:
  - Modular Programming
  - Structures ("struct")
  - Functions
  - Arrays
  - Pointers
  - String Manipulation
  - Conditional Statements
  - Looping
  - File Handling

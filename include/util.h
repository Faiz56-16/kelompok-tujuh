/* util.h - Input console yang aman & fungsi format (tanpa logika bisnis). */
#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <time.h>

/* Input */
void baca_baris(const char *prompt, char *buf, size_t n);
int  baca_int(const char *prompt, int min, int max);  /* min-1 jika EOF */
void tekan_enter(void);
int  input_habis(void);                               /* 1 jika stdin EOF */

/* Format */
void format_durasi(long detik, char *out, size_t sz);   /* HH:MM:SS */
void format_rupiah(long n, char *out, size_t sz);       /* Rp12.500 */
void format_waktu(time_t t, char *out, size_t sz);      /* dd-mm-yyyy HH:MM:SS */

#endif

/* util.c - Input console yang aman & fungsi format. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

static int eof_input = 0;

int input_habis(void) { return eof_input; }

void baca_baris(const char *prompt, char *buf, size_t n) {
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(buf, (int)n, stdin)) {
        buf[0] = '\0';
        eof_input = 1;
        return;
    }
    if (!strchr(buf, '\n')) {            /* baris terlalu panjang: buang sisanya */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    buf[strcspn(buf, "\r\n")] = '\0';
}

int baca_int(const char *prompt, int min, int max) {
    char buf[64];
    for (;;) {
        char *akhir;
        long v;
        baca_baris(prompt, buf, sizeof(buf));
        if (eof_input) return min - 1;
        v = strtol(buf, &akhir, 10);
        if (akhir != buf && *akhir == '\0' && v >= min && v <= max)
            return (int)v;
        printf("  Input tidak valid. Masukkan angka %d-%d.\n", min, max);
    }
}

void tekan_enter(void) {
    char buf[8];
    baca_baris("\nTekan ENTER untuk kembali ke menu...", buf, sizeof(buf));
}

void format_durasi(long detik, char *out, size_t sz) {
    if (detik < 0) detik = 0;
    snprintf(out, sz, "%02ld:%02ld:%02ld",
             detik / 3600, (detik % 3600) / 60, detik % 60);
}

void format_rupiah(long n, char *out, size_t sz) {
    char tmp[32], res[48];
    int len, i, j = 0;
    snprintf(tmp, sizeof(tmp), "%ld", n);
    len = (int)strlen(tmp);
    for (i = 0; i < len; i++) {
        if (i > 0 && (len - i) % 3 == 0) res[j++] = '.';
        res[j++] = tmp[i];
    }
    res[j] = '\0';
    snprintf(out, sz, "Rp%s", res);
}

void format_waktu(time_t t, char *out, size_t sz) {
    struct tm *tm_info = localtime(&t);
    if (tm_info) strftime(out, sz, "%d-%m-%Y %H:%M:%S", tm_info);
    else snprintf(out, sz, "-");
}

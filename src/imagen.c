#include "main.h"

/* ---------- helpers big endian ---------- */

static int leer_u32(FILE *f, uint32_t *out) {
    uint8_t b[4];
    if (fread(b, 1, 4, f) != 4) return -1;
    *out = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
           ((uint32_t)b[2] << 8)  |  (uint32_t)b[3];
    return 0;
}

static int escribir_u32(FILE *f, uint32_t v) {
    uint8_t b[4] = { v >> 24, v >> 16, v >> 8, v };
    return fwrite(b, 1, 4, f) == 4 ? 0 : -1;
}

/* ---------- carga ---------- */

int cargar_imagen(const char *nombre_archivo, MaquinaVirtual *mv) {
    FILE *f = fopen(nombre_archivo, "rb");
    if (!f) {
        fprintf(stderr, "No se pudo abrir la imagen: %s\n", nombre_archivo);
        return -1;
    }

    uint8_t hdr[8];
    if (fread(hdr, 1, 8, f) != 8 || memcmp(hdr, "VMI26", 5) != 0) {
        fprintf(stderr, "Imagen invalida: %s\n", nombre_archivo);
        fclose(f);
        return -1;
    }
    if (hdr[5] != 1) {
        fprintf(stderr, "Version de imagen no soportada: %d\n", hdr[5]);
        fclose(f);
        return -1;
    }

    uint32_t kib = ((uint32_t)hdr[6] << 8) | hdr[7];
    uint32_t tam = kib * 1024;
    if (kib == 0 || tam > 65536) {
        fprintf(stderr, "Tamaño de memoria invalido en la imagen: %u KiB\n", kib);
        fclose(f);
        return -1;
    }

    /* el tamaño sale de la imagen; m=M se ignora */
    free(mv->RAM);
    mv->RAM = calloc(tam, 1);
    if (!mv->RAM) { fclose(f); return -1; }
    mv->tamRam = tam;               /* requiere uint32_t */

    for (int i = 0; i < MR; i++)
        if (leer_u32(f, &mv->REGS[i]) != 0) goto err;

    for (int i = 0; i < MTDS; i++)
        if (leer_u32(f, &mv->TDS[i]) != 0) goto err;

    if (fread(mv->RAM, 1, tam, f) != tam) goto err;

    mv->version = 2;                /* las imagenes solo existen en MV2 */
    fclose(f);
    return 0;

err:
    fprintf(stderr, "Imagen truncada o corrupta: %s\n", nombre_archivo);
    fclose(f);
    return -1;
}

/* ---------- guardado (lo llama el breakpoint) ---------- */

int guardar_imagen(const char *nombre_archivo, MaquinaVirtual *mv) {
    FILE *f = fopen(nombre_archivo, "wb");
    if (!f) {
        fprintf(stderr, "No se pudo crear la imagen: %s\n", nombre_archivo);
        return -1;
    }

    uint16_t kib = mv->tamRam / 1024;
    uint8_t hdr[8] = { 'V','M','I','2','6', 1, kib >> 8, kib & 0xFF };
    if (fwrite(hdr, 1, 8, f) != 8) goto err;

    for (int i = 0; i < MR; i++)
        if (escribir_u32(f, mv->REGS[i]) != 0) goto err;

    for (int i = 0; i < MTDS; i++)
        if (escribir_u32(f, mv->TDS[i]) != 0) goto err;

    if (fwrite(mv->RAM, 1, mv->tamRam, f) != mv->tamRam) goto err;

    fclose(f);
    return 0;

err:
    fprintf(stderr, "Error al escribir la imagen: %s\n", nombre_archivo);
    fclose(f);
    return -1;
}
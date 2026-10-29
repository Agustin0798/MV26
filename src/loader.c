#include "main.h"
#include <errno.h>

typedef struct {
    uint8_t  version;
    uint16_t tam_codigo;
    uint16_t tam_datos;
    uint16_t tam_extra;
    uint16_t tam_pila;
    uint16_t tam_const;
    uint16_t entry;
} Cabecera;

static void error_fatal(const char *contexto, const char *detalle) {
    fprintf(stderr, "Error al cargar el programa (%s): %s\n", contexto, detalle);
    exit(EXIT_FAILURE);
}

static FILE *abrir_archivo(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) error_fatal(path, strerror(errno));
    return f;
}

/* ---------- helpers de lectura / escritura big-endian ---------- */
static uint16_t leer_u16_be(FILE *f, const char *campo) {
    uint8_t b[2];
    if (fread(b, 1, 2, f) != 2) error_fatal("cabecera", campo);
    return (uint16_t)((b[0] << 8) | b[1]);
}

static void escribir_u32_be(uint8_t *dst, uint32_t v) {
    dst[0] = v >> 24; dst[1] = v >> 16; dst[2] = v >> 8; dst[3] = v;
}

/* ---------- cabecera ---------- */
static void leer_cabecera(FILE *f, Cabecera *c, const MaquinaVirtual *mv) {
    uint8_t id[5];
    if (fread(id, 1, 5, f) != 5)
        error_fatal("cabecera", "no se pudo leer la cabecera completa");
    if (memcmp(id, "VMX26", 5) != 0)
        error_fatal("cabecera", "identificador inválido (se esperaba \"VMX26\")");

    int v = fgetc(f);
    if (v == EOF) error_fatal("cabecera", "falta el byte de versión");

    memset(c, 0, sizeof(*c));
    c->version = (uint8_t)v;

    switch (c->version) {
        case 1:
            c->tam_codigo = leer_u16_be(f, "falta tamaño de código");
            if (c->tam_codigo > mv->tamRam)
                error_fatal("cabecera", "tamaño de código excede la memoria disponible");
            c->tam_datos = (uint16_t)(mv->tamRam - c->tam_codigo); /* v1: datos = resto */
            break;
        case 2:
            c->tam_codigo = leer_u16_be(f, "tamaño code");
            c->tam_datos  = leer_u16_be(f, "tamaño data");
            c->tam_extra  = leer_u16_be(f, "tamaño extra");
            c->tam_pila   = leer_u16_be(f, "tamaño stack");
            c->tam_const  = leer_u16_be(f, "tamaño const");
            c->entry      = leer_u16_be(f, "offset entry point");
            break;
        default:
            error_fatal("cabecera", "versión de archivo no soportada");
    }
}

//  segmento de parámetros 
static uint32_t tam_param_segment(const char *vmx, int np, char **params) {
    
    /* argc = nparams + 1 (param0 = nombre del .vmx), ver nota al final */
    if (np <= 0) 
        return 0;
    uint32_t t = (uint32_t)strlen(vmx) + 1;
    for (int i = 0; i < np; i++) 
        t += (uint32_t)strlen(params[i]) + 1;
    
    return t + 4u * (uint32_t)(np + 1);
}

static void cargar_param_segment(MaquinaVirtual *mv, uint32_t base, const char *vmx, int np, char **params, uint32_t *off_argv) {
    uint8_t *p = mv->RAM + base;
    uint32_t off = 0;
    int cant = np + 1;                          /* punteros en argv (param0 = .vmx) */
    uint32_t *offs = malloc(cant * sizeof(uint32_t));
    if (!offs) error_fatal("param segment", "sin memoria");

    offs[0] = off;
    strcpy((char *)p + off, vmx);
    off += (uint32_t)strlen(vmx) + 1;
    for (int i = 0; i < np; i++) {
        offs[i + 1] = off;
        strcpy((char *)p + off, params[i]);
        off += (uint32_t)strlen(params[i]) + 1;
    }
    *off_argv = off;
    for (int i = 0; i < cant; i++)
        escribir_u32_be(p + off + 4 * i, offs[i]);

    free(offs);
}

// TDS + registros de segmento
static void armar_segmentos(MaquinaVirtual *mv, const Cabecera *c, uint32_t tam_ps, uint32_t base_fis[32]) {
    
    // orden físico: PS, KS, CS, DS, ES, SS
    const int      regs[6]  = { PS, KS, CS, DS, ES, SS };
    const uint32_t tams[6]  = { tam_ps, c->tam_const, c->tam_codigo, c->tam_datos, c->tam_extra, c->tam_pila };

    for (int i = 0; i < MTDS; i++) mv->TDS[i] = -1;

    uint32_t base = 0;
    int n = 0;
    for (int i = 0; i < 6; i++) {
        if (tams[i] == 0) { mv->REGS[regs[i]] = -1; continue; }
        if (base + tams[i] > mv->tamRam)
            error_fatal("memoria", "memoria insuficiente para alojar el proceso");
        mv->TDS[n]        = empaquetar32((uint16_t)base, (uint16_t)tams[i]);
        mv->REGS[regs[i]] = empaquetar32((uint16_t)n, 0);
        base_fis[regs[i]] = base;
        base += tams[i];
        n++;
    }
}

//carga de contenido
static void leer_bloque(FILE *f, uint8_t *dst, uint32_t n, const char *que) {
    if (n && fread(dst, 1, n, f) != n)
        error_fatal(que, "el archivo no contiene todos los bytes declarados en la cabecera");
}

//pila inicial (solo v2)
static void inicializar_pila(MaquinaVirtual *mv, const Cabecera *c, const uint32_t base_fis[32], int np, uint32_t off_argv) {
    if (c->tam_pila < 12) error_fatal("pila", "stack overflow al preparar la pila inicial");

    uint32_t argc = np > 0 ? (uint32_t)(np + 1) : 0;
    uint32_t argv = np > 0 ? (uint32_t)(mv->REGS[PS] + off_argv) : 0xFFFFFFFFu;

    uint8_t *tope = mv->RAM + base_fis[SS] + c->tam_pila;
    escribir_u32_be(tope - 4,  argv);          /* SP+8  */
    escribir_u32_be(tope - 8,  argc);          /* SP+4  */
    escribir_u32_be(tope - 12, 0xFFFFFFFFu);   /* SP    : retorno = -1 */

    mv->REGS[SP] = mv->REGS[SS] + c->tam_pila - 12;
}

void cargar_programa(const char *path, MaquinaVirtual *mv, int nparams, char **params) {
    FILE *f = abrir_archivo(path);
    Cabecera c;
    uint32_t base_fis[32] = {0};
    uint32_t off_argv = 0;

    leer_cabecera(f, &c, mv);
    mv->version=c.version;
    
    memset(mv->REGS, 0, sizeof(mv->REGS));
    memset(mv->RAM, 0, mv->tamRam);

    uint32_t tam_ps = (c.version >= 2) ? tam_param_segment(path, nparams, params) : 0;
    if (c.version == 1) nparams = 0;

    armar_segmentos(mv, &c, tam_ps, base_fis);

    leer_bloque(f, mv->RAM + base_fis[CS], c.tam_codigo, "codigo");
    if (c.tam_const)
        leer_bloque(f, mv->RAM + base_fis[KS], c.tam_const, "const segment");
    if (tam_ps)
        cargar_param_segment(mv, base_fis[PS], path, nparams, params, &off_argv);

    mv->REGS[IP] = mv->REGS[CS] | c.entry;     /* v1: entry = 0 → IP = CS */

    if (c.version == 2)
        inicializar_pila(mv, &c, base_fis, nparams, off_argv);

    fclose(f);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mv.h"
#include "loader.h"


//  Manejo centralizado de errores 
static void error_fatal(const char *contexto, const char *detalle) {
    fprintf(stderr, "Error al cargar el programa (%s): %s\n", contexto, detalle);
    exit(EXIT_FAILURE);
}

/* ---------- Apertura del archivo ---------- */
static FILE *abrir_archivo(const char *path) {
    FILE *archivo = fopen(path, "rb");
    if (archivo == NULL) {
        error_fatal(path, strerror(errno));
    }
    return archivo;
}


//Lectura de un entero de 16 bits en big-endian
static uint16_t leer_u16_be(const uint8_t *bytes) {
    return (uint16_t)((bytes[0] << 8) | bytes[1]);
}

/* Lectura y validación de la cabecera 
 * Devuelve el tamaño del código (en bytes) declarado en la cabecera.
 * Si algo no coincide con lo esperado, aborta con error_fatal.
 */
void leer_cabecera(FILE *archivo, int *version, uint16_t *tam_codigo) {
    uint8_t cabecera[4];
    size_t leidos = fread(cabecera, 1, sizeof(cabecera), archivo);
    if (leidos != sizeof(cabecera)) {
        error_fatal("cabecera", "no se pudo leer la cabecera completa");
    }

    *version = cabecera[0];
    if (*version != 1) {
        error_fatal("cabecera", "versión de archivo no soportada");
    }

    *tam_codigo = leer_u16_be(&cabecera[2]);
    if (*tam_codigo > MM) {
        error_fatal("cabecera", "tamaño de código excede la memoria disponible");
    }
}

void inicializar_tds(MaquinaVirtual *mv, int version, uint16_t tam_codigo) {

    for (int i = 0; i < MTDS; i++) {
        mv->TDS[i] = -1;
    }
    
    switch (version) {
        case 1:
            mv->TDS[0] = empaquetar32(0, tam_codigo);
            mv->TDS[1] = empaquetar32(1, MM - tam_codigo);
            break;
        default:
            error_fatal("inicializar_tds", "versión de archivo no soportada");
    }
    
}

// Carga del código en memoria
void cargar_codigo_en_memoria(FILE *archivo, MaquinaVirtual *mv, uint16_t tam_codigo) {
    size_t leidos = fread(mv->RAM, 1, tam_codigo, archivo);
    if (leidos != tam_codigo) {
        error_fatal("codigo",
            "el archivo no contiene todos los bytes de codigo declarados en la cabecera");
    }
}

// Inicialización de registros
static void inicializar_registros(MaquinaVirtual *mv, int version) {
    memset(mv->REGS, 0, sizeof(mv->REGS));

    switch(version) {
        case 1:
            mv->REGS[CS] = empaquetar32(0, 0); /* 00 00 00 00 */
            mv->REGS[DS] = empaquetar32(1, 0);  /* 00 01 00 00 */
            mv->REGS[IP] = mv->REGS[CS];
            break;
        default:
            error_fatal("inicializar_registros", "versión de archivo no soportada");
    }
    mv->REGS[CS] = empaquetar32(0, 0); /* 00 00 00 00 */
    mv->REGS[DS] = empaquetar32(1, 0);  /* 00 01 00 00 */
    mv->REGS[IP] = mv->REGS[CS];
}


void cargar_programa(const char *path, MaquinaVirtual *mv) {
    
    FILE *archivo = abrir_archivo(path);
    int version = 0;
    uint16_t tam_codigo;
    
    leer_cabecera(archivo, &version, &tam_codigo);

    inicializar_tds(mv, version, tam_codigo);
    cargar_codigo_en_memoria(archivo, mv, tam_codigo);
    inicializar_registros(mv);

    fclose(archivo);
}

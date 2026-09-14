#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "funct.h"
#include "logic.h"

//constantes de cantidad
#define MM 16384
#define MR 32
#define MTDS 8
#define MF 32

//constantes de registros
#define IP  0
#define OPC 1
#define OP1 2
#define OP2 3
#define LAR 4
#define MAR 5
#define MBR 6

#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15
#define AC  16
#define CC  17


#define CS  26
#define DS  27

//definiciones de variables globales
extern void (*func[MF])(int32_t *,int32_t *);

/* ---------- Estado completo de la máquina virtual ---------- */

typedef struct {
    uint8_t RAM[MM];
    uint32_t TDS[MTDS];
    int32_t REGS[MR];
} MaquinaVirtual;

/* ---------- Helpers de empaquetado de 32 bits ----------
 * Varios campos de la especificación (entradas de la TDS, direcciones
 * lógicas, OP1/OP2, etc.) son un entero de 32 bits compuesto por dos
 * mitades de 16 bits: la mitad alta y la mitad baja. Se centralizan acá
 * porque loader.c y, más adelante, cpu.c van a necesitar exactamente lo
 * mismo (por ejemplo, para armar CS/DS o para traducir direcciones
 * lógicas a físicas).
 */

static inline int32_t empaquetar32(uint16_t mitad_alta, uint16_t mitad_baja) {
    return (int32_t)(((uint32_t)mitad_alta << 16) | (uint32_t)mitad_baja);
}

static inline uint16_t mitad_alta(int32_t valor) {
    return (uint16_t)(((uint32_t)valor) >> 16);
}

static inline uint16_t mitad_baja(int32_t valor) {
    return (uint16_t)(((uint32_t)valor) & 0xFFFFu);
}
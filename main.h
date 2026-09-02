#pragma once

#include <stdint.h>
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
extern int8_t RAM[MM];
extern int32_t REGS[MR];
extern int32_t TDS[MTDS];
extern void (*func[MF])(int32_t *,int32_t *);

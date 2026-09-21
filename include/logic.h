#pragma once

// constantes de errores
#define InstInv 1
#define DivCero 2
#define FalloSeg 3

void error(int ce);
int32_t valorOperado(int op, MaquinaVirtual *mv);
void infoOperando(int op, uint32_t dirFisOP, MaquinaVirtual *mv);
void decoOperacion(uint8_t operacion, MaquinaVirtual *mv);
void guardaMem(uint32_t dirFis, uint32_t puntero, int cant, uint32_t valor, MaquinaVirtual *mv);
int32_t leeMem(uint32_t dirFis, uint32_t puntero, int cant, MaquinaVirtual *mv);
uint32_t calculaDirFis(uint32_t puntero, int16_t offset, MaquinaVirtual *mv);
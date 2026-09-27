#pragma once

// constantes de errores
#define InstInv 1
#define DivCero 2
#define FalloSeg 3

void dirValida(uint32_t puntero, int16_t dirFis, MaquinaVirtual *mv);
int16_t calculaDirFis(int16_t offset, uint32_t puntero, MaquinaVirtual *mv);
void guardaMem(MaquinaVirtual *mv);
void leeMem(MaquinaVirtual *mv);
int32_t valorOperado(int op, MaquinaVirtual *mv);
void infoOperando(MaquinaVirtual *mv);
void decoOperacion(uint8_t operacion, MaquinaVirtual *mv);
void leeOperacion(uint8_t *operacion, MaquinaVirtual *mv);
void error(int ce);
void guardaOP(uint8_t op, uint32_t valor, MaquinaVirtual *mv);
void decideGuardar(uint32_t valor1, uint32_t valor2, MaquinaVirtual *mv);
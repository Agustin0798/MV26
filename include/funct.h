#pragma once

void modificaCC(int32_t);

// Función para los códigos no definidos
void NULA(int32_t *a, int32_t *b, MaquinaVirtual *mv);

// Prototipos de las instrucciones
void SYS(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JMP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JN(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JZ(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JC(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JV(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JNP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JNN(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JNZ(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void NOT(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void STOP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void MOV(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void ADD(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SUB(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void MUL(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void DIV(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void CMP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void AND(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void OR(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void XOR(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SWAP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SHL(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SHR(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SAR(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void LDL(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void LDH(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void RND(int32_t *a, int32_t *b, MaquinaVirtual *mv);
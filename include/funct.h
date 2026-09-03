#pragma once

void modificaCC(int32_t);

// Función para los códigos no definidos
void NULA(int32_t *a, int32_t *b);

// Prototipos de las instrucciones
void SYS(int32_t *a, int32_t *b);
void JMP(int32_t *a, int32_t *b);
void JP(int32_t *a, int32_t *b);
void JN(int32_t *a, int32_t *b);
void JZ(int32_t *a, int32_t *b);
void JC(int32_t *a, int32_t *b);
void JV(int32_t *a, int32_t *b);
void JNP(int32_t *a, int32_t *b);
void JNN(int32_t *a, int32_t *b);
void JNZ(int32_t *a, int32_t *b);
void NOT(int32_t *a, int32_t *b);
void STOP(int32_t *a, int32_t *b);
void MOV(int32_t *a, int32_t *b);
void ADD(int32_t *a, int32_t *b);
void SUB(int32_t *a, int32_t *b);
void MUL(int32_t *a, int32_t *b);
void DIV(int32_t *a, int32_t *b);
void CMP(int32_t *a, int32_t *b);
void AND(int32_t *a, int32_t *b);
void OR(int32_t *a, int32_t *b);
void XOR(int32_t *a, int32_t *b);
void SWAP(int32_t *a, int32_t *b);
void SHL(int32_t *a, int32_t *b);
void SHR(int32_t *a, int32_t *b);
void SAR(int32_t *a, int32_t *b);
void LDL(int32_t *a, int32_t *b);
void LDH(int32_t *a, int32_t *b);
void RND(int32_t *a, int32_t *b);
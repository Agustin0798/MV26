#include "main.h"

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

void MOV(int32_t *a, int32_t *b)
{
    *a=*b;
    modificaCC(*a);
}

void ADD(int32_t *a, int32_t *b)
{
    *a+=*b;
    modificaCC(*a);
}

void SUB(int32_t *a, int32_t *b)
{
    *a-=*b;
    modificaCC(*a);
}

void MUL(int32_t *a, int32_t *b)
{
    (*a)*=*b;
    modificaCC(*a);
}

void DIV(int32_t *a, int32_t *b)
{
    int32_t cociente,resto;

    if(*b != 0)
    {
        cociente=*a / *b;
        resto=*a % *b;

        *a=cociente;
        REGS[AC]=resto;
    }
    else
        error(DivCero);
}

void CMP(int32_t *a, int32_t *b)
{
    int32_t resta=*a - *b;
    modificaCC(resta);
}

void AND(int32_t *a, int32_t *b)
{
    *a=*a & *b;
    modificaCC(*a);
}

void OR(int32_t *a, int32_t *b)
{
    *a=*a | *b;
    modificaCC(*a);
}

void XOR(int32_t *a, int32_t *b)
{
    *a=*a ^ *b;
    modificaCC(*a);
}

void SWAP(int32_t *a, int32_t *b);

void SHL(int32_t *a, int32_t *b)
{
    uint32_t resultado=*a << *b;
    *a = resultado;
    modificaCC(resultado);
}

void SHR(int32_t *a, int32_t *b)
{
    uint32_t resultado= *a >> *b;
    *a=resultado;
    modificaCC(resultado);
}

void SAR(int32_t *a, int32_t *b)
{
    *a= *a >> *b;
    modificaCC(*a);
}

#include <stdint.h>

void LDH(int32_t *a, int32_t *b) 
{ 
    *a = (*a & 0x0000FFFF) | ((*b & 0x0000FFFF) << 16);
}

void LDL(int32_t *a, int32_t *b) 
{
    *a = (*a & 0xFFFF0000) | (*b & 0x0000FFFF);
}

void RND(int32_t *a, int32_t *b);
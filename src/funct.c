#include "main.h"

void modificaCC(int32_t);

// Función para los códigos no definidos
void NULA(int32_t *a, int32_t *b)
{
    error(InstInv);
}

// Prototipos de las instrucciones
void SYS(int32_t *a, int32_t *b);

void JMP(int32_t *a, int32_t *b)
{
    LDL(REGS[IP],b);
}

void JP(int32_t *a, int32_t *b)
{
    uint32_t aux=REGS[CC];

    aux>>=28;
    if ((aux & 0b1100) == 0) //bits N y Z apagados (no se consideran los bits C y V)
        JMP(a,b);
}

void JN(int32_t *a, int32_t *b)
{
    uint32_t aux=REGS[CC];

    aux>>=28;
    if ((aux & 0b1100) == 0b1000) //bit N prendido y Z apagado
        JMP(a,b);
}

void JZ(int32_t *a, int32_t *b)
{
    uint32_t aux=REGS[CC];

    aux>>=28;
    if ((aux & 0b1100) == 0b0100) //bit N apagado y Z prendido
        JMP(a,b);
}

void JC(int32_t *a, int32_t *b)
{
    uint32_t aux=REGS[CC];

    aux>>=28;
    if ((aux & 0b0010) == 0b0010) //bit C encendido (el resto son irrelevantes)
        JMP(a,b);
}

void JV(int32_t *a, int32_t *b)
{
    uint32_t aux=REGS[CC];

    aux>>=28;
    if ((aux & 0b0001) == 0b0001) //bit V encendido (el resto son irrelevantes)
        JMP(a,b);
}

void JNP(int32_t *a, int32_t *b)
{
    uint32_t aux=REGS[CC];

    aux>>=28;
    aux&=0b1100;
    if ((aux == 0b1000) || (aux == 0b0100)) //bit N encendido o bit Z encendido
        JMP(a,b);
}

void JNN(int32_t *a, int32_t *b)
{
    uint32_t aux=REGS[CC];

    aux>>=28;
    if ((aux & 0b1000) == 0) // bit N apagado (el resto son irrelevantes)
        JMP(a,b);
}

void JNZ(int32_t *a, int32_t *b)
{
    uint32_t aux=REGS[CC];

    aux>>=28;
    if ((aux & 0b0100) == 0) //bit Z apagado (el resto son irrelevantes)
        JMP(a,b);
}

void NOT(int32_t *a, int32_t *b)
{
    ~(*b);
}

void STOP(int32_t *a, int32_t *b)
{
    REGS[IP]=-1;
}

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

void LDH(int32_t *a, int32_t *b) 
{ 
    *a = (*a & 0x0000FFFF) | ((*b & 0x0000FFFF) << 16);
}

void LDL(int32_t *a, int32_t *b) 
{
    *a = (*a & 0xFFFF0000) | (*b & 0x0000FFFF);
}

void RND(int32_t *a, int32_t *b)
{
    srand(time(NULL));
    *a=rand() % (*b + 1);
}
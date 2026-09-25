#include "mv.h"

void modificaCC(int32_t ori, int64_t over, uint64_t carry,MaquinaVirtual *mv)
{
    if (ori == 0)
        mv->REGS[CC]=0x40000000;
    else if (ori < 0)
        mv->REGS[CC]=0x80000000;
    else
        mv->REGS[CC]=0;

    if (over != ori)
        mv->REGS[CC]=mv->REGS[CC] | 0x10000000;
    carry=carry & 0xFFFFFFFF00000000;
    if (carry > 0)
        mv->REGS[CC]=mv->REGS[CC] | 0x20000000;
}

// Función para los códigos no definidos
void NULA(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    error(InstInv);
}

// Prototipos de las instrucciones
void SYS(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t punt_inicio=mv->REGS[EDX],dirFis;
    uint32_t cant_val=mv->REGS[ECX] & 0x0000FFFF;
    uint32_t tam_val=(mv->REGS[ECX] >> 16) & 0x0000FFFF;
    uint32_t formato=mv->REGS[EAX];
    uint32_t buffer;
    char aux[33];
    int i;

    switch (*b)
    {
        case 1: //READ
                for (i=0; i<cant_val;i++)
                {
                    switch (formato)
                    {
                        case 0x01:  //DECIMAL
                                scanf(" %d",&buffer);
                            break;
                        case 0x02: //CARACTER
                                scanf(" %c",&buffer);
                            break;
                        case 0x04: //OCTAL
                                scanf(" %o",&buffer);
                            break;
                        case 0x08: //HEXADECIMAL
                                scanf(" %x",&buffer);
                            break;
                        case 0x10: //BINARIO
                                scanf(" %32s",aux);
                                buffer=strtol(aux,NULL,2); //estandar para lectura de nums binarios
                            break;
                        
                        default:
                                printf("\nFORMATO DE READ INVALIDO\n");
                            break;
                    }
                    //guardar en memoria
                    //completar cuando tengamos las funciones para trabajar en memoria
                }
            break;
        case 2: //WRITE
                for (i=0; i < cant_val; i++)
                {
                    //traer de memoria
                    //completar cuando tengamos las funciones para trabajar en memoria
                    if ((formato & 0b00001) == 0b00001) //DECIMAL
                    {
                        printf("%d",&buffer);
                    }
                    if ((formato & 0b00010) == 0b00010) //CARACTER
                    {
                        printf("%c",&buffer);
                    }
                    if ((formato & 0b00100) == 0b00100) //OCTAL
                    {
                        printf("%o",&buffer);
                    }
                    if ((formato & 0b01000) == 0b01000) //HEXADECIMAL
                    {
                        printf("%x",&buffer);
                    }
                    if ((formato & 0b10000) == 0b10000) //BINARIO
                    {
                        int bit,i;
                        int cant_bits= sizeof(buffer) *8;
                        for (i = cant_bits - 1; i >= 0; i--)
                        {
        
                            bit = (buffer >> i) & 1;
                            printf("%d", bit);
                        }
        
                    }
                    printf("\n");
                }
            break;
        default:
            break;
    }
}

void JMP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    LDL(mv->REGS[IP],b,mv);
}

void JP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t aux=mv->REGS[CC];

    aux>>=28;
    if ((aux & 0b1100) == 0) //bits N y Z apagados (no se consideran los bits C y V)
        JMP(a,b,mv);
}

void JN(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t aux=mv->REGS[CC];

    aux>>=28;
    if ((aux & 0b1100) == 0b1000) //bit N prendido y Z apagado
        JMP(a,b,mv);
}

void JZ(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t aux=mv->REGS[CC];

    aux>>=28;
    if ((aux & 0b1100) == 0b0100) //bit N apagado y Z prendido
        JMP(a,b,mv);
}

void JC(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t aux=mv->REGS[CC];

    aux>>=28;
    if ((aux & 0b0010) == 0b0010) //bit C encendido (el resto son irrelevantes)
        JMP(a,b,mv);
}

void JV(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t aux=mv->REGS[CC];

    aux>>=28;
    if ((aux & 0b0001) == 0b0001) //bit V encendido (el resto son irrelevantes)
        JMP(a,b,mv);
}

void JNP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t aux=mv->REGS[CC];

    aux>>=28;
    aux&=0b1100;
    if ((aux == 0b1000) || (aux == 0b0100)) //bit N encendido o bit Z encendido
        JMP(a,b,mv);
}

void JNN(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t aux=mv->REGS[CC];

    aux>>=28;
    aux&=0b1100;
    if ((aux == 0b0100) || (aux == 0b0000)) //bit Z encendido o ambos apagados
        JMP(a,b,mv);
}


void JNZ(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t aux=mv->REGS[CC];

    aux>>=28;
    if ((aux & 0b0100) == 0) //bit Z apagado (el resto son irrelevantes)
        JMP(a,b,mv);
}

void NOT(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c2=*b;
    int64_t o2=*b;

    c2=~(*b);
    o2=~(*b);
    *b=o2 & 0x00000000FFFFFFFF;
    modificaCC(*a,o2,c2,mv);
    *b = ~(*b);
}


void STOP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    mv->REGS[IP]= -1;
}

void MOV(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1=c2;
    o1=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void ADD(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1+=c2;
    o1+=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void SUB(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=~(*b)+1;
    int64_t o1=*a,o2=~(*b)+1;

    c1+=c2;
    o1+=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void MUL(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1*=c2;
    o1*=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void DIV(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    if(*b != 0)
    {
        int32_t resto;
        uint64_t c1=*a,c2=*b;
        int64_t o1=*a,o2=*b;
        
        resto=*a % *b;
        mv->REGS[AC]=resto;
        c1/=c2;
        o1/=o2;
        *a=o1 & 0x00000000FFFFFFFF;
        modificaCC(*a,o1,c1,mv);
    }
    else
        error(DivCero);
}

void CMP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    int32_t resta;
    uint64_t c1=*a,c2=~(*b)+1;
    int64_t o1=*a,o2=~(*b)+1;

    c1+=c2;
    o1+=o2;
    resta=o1 & 0x00000000FFFFFFFF;
    modificaCC(resta,o1,c1,mv);
}

void AND(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1&=c2;
    o1&=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void OR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1|=c2;
    o1|=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void XOR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1^=c2;
    o1^=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void SWAP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1^=c2;
    c2^=c1;
    c1^=c2;

    o1^=o2;
    o2^=o1;
    o1^=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
    //TODO: ver si se modifica CC en este caso, no se especifica en el enunciado
}

void SHL(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1<<=c2;
    o1<<=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void SHR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1>>=c2;
    o1>>=o2;
    *a=c1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void SAR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    c1>>=c2;
    o1>>=o2;
    *a=o1 & 0x00000000FFFFFFFF;
    modificaCC(*a,o1,c1,mv);
}

void LDH(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{ 
    *a = (*a & 0x0000FFFF) | ((*b & 0x0000FFFF) << 16);
}

void LDL(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{
    *a = (*a & 0xFFFF0000) | (*b & 0x0000FFFF);
}

void RND(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    srand(time(NULL));
    *a=rand() % (*b + 1);
}
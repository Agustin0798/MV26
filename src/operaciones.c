#include "mv.h"

void modificaCC(int32_t);

// Función para los códigos no definidos
void NULA(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    error(InstInv);
}

// Prototipos de las instrucciones
void SYS(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t punt_inicio=mv->REGS[EDX];
    int16_t dirFis;
    uint16_t cant_val=mv->REGS[ECX] & 0x0000FFFF;
    uint16_t tam_val=(mv->REGS[ECX] >> 16) & 0x0000FFFF;
    uint32_t formato=mv->REGS[EAX];
    int32_t buffer;
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
                    dirFis=calculaDirFis(i*tam_val,punt_inicio,mv);
                    mv->REGS[LAR]=punt_inicio;
                    mv->REGS[MAR]= (tam_val << 16) | dirFis;
                    mv->REGS[MBR]=buffer;
                    guardaMem(mv);
                }
            break;
        case 2: //WRITE
                for (i=0; i < cant_val; i++)
                {
                    dirFis=calculaDirFis(i*tam_val,punt_inicio,mv);
                    mv->REGS[LAR]=punt_inicio;
                    mv->REGS[MAR]= (tam_val << 16) | dirFis;
                    leeMem(mv);
                    buffer=mv->REGS[MBR];
                    
                    if (tam_val < 4)
                    {
                        buffer= buffer << (4 - tam_val);
                        buffer= buffer >> (4 - tam_val);
                    }

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
    *b = ~(*b);
}


void STOP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    mv->REGS[IP]= -1;
}

void MOV(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a=*b;
    modificaCC(*a);
}

void ADD(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a+=*b;
    modificaCC(*a);
}

void SUB(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a-=*b;
    modificaCC(*a);
}

void MUL(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    (*a)*=*b;
    modificaCC(*a);
}

void DIV(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    int32_t cociente,resto;

    if(*b != 0)
    {
        cociente=*a / *b;
        resto=*a % *b;

        *a=cociente;
        mv->REGS[AC]=resto;
    }
    else
        error(DivCero);
}

void CMP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    int32_t resta=*a - *b;
    modificaCC(resta);
}

void AND(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a=*a & *b;
    modificaCC(*a);
}

void OR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a=*a | *b;
    modificaCC(*a);
}

void XOR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a=*a ^ *b;
    modificaCC(*a);
}

void SWAP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    int32_t temp=*a;
    *a=*b;
    *b=temp;
    //TODO: ver si se modifica CC en este caso, no se especifica en el enunciado
}

void SHL(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t resultado=*a << *b;
    *a = resultado;
    modificaCC(resultado);
}

void SHR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t resultado= *a >> *b;
    *a=resultado;
    modificaCC(resultado);
}

void SAR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a= *a >> *b;
    modificaCC(*a);
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
#include "main.h"
#define U(x) ((uint64_t)(uint32_t)(x))   // para carry: sin extensión de signo
#define S(x) ((int64_t)(int32_t)(x))     // para overflow: con extensión


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
    uint32_t punt_inicio=mv->REGS[EDX];
    uint16_t dirFis;
    uint16_t cant_val=mv->REGS[ECX] & 0x0000FFFF;
    uint16_t tam_val=(mv->REGS[ECX] >> 16) & 0x0000FFFF;
    uint32_t formato=mv->REGS[EAX];
    int32_t buffer=0;
    char aux[33],* cadena,byte;
    int i;

    switch (*b)
    {
        case 1: //READ
                for (i=0; i<cant_val;i++)
                {
                    dirFis=calculaDirFis(i*tam_val,punt_inicio,mv);
                    mv->REGS[LAR]=punt_inicio;
                    mv->REGS[MAR]= ((uint32_t)tam_val << 16) | (dirFis & 0xFFFF);
                    printf("[%04X]: ", dirFis);

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
                    mv->REGS[MBR]=buffer;
                    guardaMem(mv);
                }
            break;
        case 2: //WRITE
                for (i = 0; i < cant_val; i++)
                {
                    dirFis = calculaDirFis(i * tam_val, punt_inicio, mv);
                    mv->REGS[LAR]= punt_inicio;
                    mv->REGS[MAR]= ((uint32_t)tam_val << 16) | (dirFis & 0xFFFF);
                    leeMem(mv);
                    uint32_t valor= mv->REGS[MBR];

                    printf("[%04X] ", dirFis);

                    if (formato & 0x10) //BINARIO (sin ceros a la izquierda)
                    {
                        printf("0b");
                        if (valor == 0)
                            printf("0");
                        else
                        {
                            int bit= 31;
                            while (((valor >> bit) & 1) == 0)
                                bit--;
                            for (; bit >= 0; bit--)
                                printf("%d", (valor >> bit) & 1);
                        }
                        printf(" ");
                    }
                    if (formato & 0x08) //HEXADECIMAL
                        printf("0x%X ", valor);
                    if (formato & 0x04) //OCTAL
                        printf("0o%o ", valor);
                    if (formato & 0x02) //CARACTER: un caracter por byte, del más al menos significativo
                    {
                        for (int k= tam_val - 1; k >= 0; k--)
                        {
                            uint8_t c= (valor >> (k * 8)) & 0xFF;
                            putchar((c >= 32 && c < 127) ? c : '.');
                        }
                        printf(" ");
                    }
                    if (formato & 0x01) //DECIMAL
                        printf("%d ", (int32_t)valor);

                    printf("\n");
                }
            break;
        case 3: //STRING READ
            {
                cant_val= mv->REGS[ECX];
                dirFis=calculaDirFis(0,punt_inicio,mv);
                if (cant_val < 0)
                    cant_val= 9999;
                cadena= (char *) malloc((cant_val*sizeof(char))+1);
                printf("[%04X]: ",dirFis);
                fgets(cadena, cant_val+1, stdin);
                mv->REGS[LAR]= punt_inicio;
                mv->REGS[MAR]= ((uint32_t)1 << 16);
                i=0;
                while ((i < cant_val) && (cadena[i] != '\0'))
                {
                    mv->REGS[MAR]= mv->REGS[MAR] | ((dirFis + i) & 0xFFFF);
                    mv->REGS[MBR]= cadena[i];
                    guardaMem(mv);
                    i++;
                }
                mv->REGS[MAR]= mv->REGS[MAR] | ((dirFis + i) & 0xFFFF);
                mv->REGS[MBR]='\0';
                guardaMem(mv);

            }
            break;
        case 4: 
            {
                cant_val= 9999;
                cadena= (char *) malloc((cant_val*sizeof(char)));
                dirFis= calculaDirFis(0,punt_inicio,mv);
                mv->REGS[LAR]= punt_inicio;
                mv->REGS[MAR]= ((uint32_t)1 << 16) | (dirFis & 0xFFFF);
                leeMem(mv);
                byte= mv->REGS[MBR] & 0x000000FF;
                i=0;
                while (byte != '\0' && i < cant_val - 1)
                {
                    cadena[i]=byte;
                    i++;
                    mv->REGS[MAR]= ((uint32_t)1 << 16) | ((dirFis+i) & 0xFFFF);
                    leeMem(mv);
                    byte= mv->REGS[MBR] & 0x000000FF;
                }
                cadena[i]='\0';
                printf("%s",cadena);
            }
            break;
        case 0: //CLEAR SCREEN
                printf("\x1b[2J\x1b[H");
            break;
        case 0x0F://BREAKPOINT
            break;
        default:
            break;
    }
}

void JMP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    LDL(&mv->REGS[IP],b,mv);
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
    if ((aux & 0b1000) == 0b0000) //bit Z encendido o ambos apagados
        JMP(a,b,mv);
}


void JNZ(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint32_t aux=mv->REGS[CC];

    aux>>=28;
    if ((aux & 0b0100) == 0b0000) //bit Z apagado (el resto son irrelevantes)
        JMP(a,b,mv);
}

void NOT(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *b=~(*b);
    modificaCC(*b, *b, 0, mv);
}

void PUSH(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    mv->REGS[SP]-= 4;
    if (mv->REGS[SP] < mv->REGS[SS]) 
        error(SOver);
    mv->REGS[LAR]= mv->REGS[SP];
    uint16_t dirFis= calculaDirFis(0,mv->REGS[SP],mv);
    mv->REGS[MAR]= ((uint32_t)4 << 16) | (dirFis & 0xFFFF);
    mv->REGS[MBR]= *b;
    guardaMem(mv);
}

void POP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    mv->REGS[LAR]= mv->REGS[SP];
    uint16_t dirFis= calculaDirFis(0,mv->REGS[SP],mv);
    mv->REGS[MAR]= ((uint32_t)4 << 16) | (dirFis & 0xFFFF);
    leeMem(mv); //detecta internamente si hay stack underflow
    *b=mv->REGS[MBR];
    mv->REGS[SP]+=4;
}

void CALL(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    PUSH(0,&mv->REGS[IP],mv);
    JMP(0,b,mv);
}

void RET(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    POP(0,&mv->REGS[IP],mv);
}

void STOP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    mv->REGS[IP]= -1;
}

void MOV(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{
    *a = *b;
    modificaCC(*a, *a, 0, mv);
}

void ADD(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{
    uint64_t c = U(*a) + U(*b);
    int64_t  o = S(*a) + S(*b);
    *a = (int32_t)o;
    modificaCC(*a, o, c, mv);
}

void SUB(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{
    uint64_t c = U(*a) + (uint32_t)(-S(*b));
    int64_t  o = S(*a) - S(*b);
    *a = (int32_t)o;
    modificaCC(*a, o, c, mv);
}

void MUL(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c = U(*a) * U(*b);
    int64_t  o = S(*a) * S(*b);
    *a = (int32_t)o;
    modificaCC(*a, o, c, mv);
}

void DIV(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{
    if (*b == 0) error(DivCero);
    int64_t o = S(*a) / S(*b);      
    uint64_t c= U(*a) / U(*b);           
    mv->REGS[AC] = (uint32_t)(S(*a) % S(*b));
    *a = (int32_t)o;
    modificaCC(*a, o, c, mv);          
}

void CMP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c = U(*a) + (uint32_t)(-S(*b));
    int64_t  o = S(*a) - S(*b);
    int32_t resta = (int32_t)o;
    modificaCC(resta, o, c, mv);
}

void AND(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{
    *a = *a & *b;
    modificaCC(*a, *a, 0, mv);
}

void OR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a = *a | *b;
    modificaCC(*a, *a, 0, mv);
}

void XOR(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a = *a ^ *b;
    modificaCC(*a, *a, 0, mv);
}

void SWAP(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    uint64_t c1=*a,c2=*b;
    int64_t o1=*a,o2=*b;

    (*a)^=(*b);
    (*b)^=(*a);
    (*a)^=(*b);

    modificaCC(*a, *a, 0, mv);
}

void SHL(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{
    uint32_t n = (uint32_t)*b;
    uint64_t c = (n >= 64) ? 0 : U(*a) << n;                        // acarreo: sin extensión de signo
    int64_t  o = (n >= 64) ? 0 : (int64_t)((uint64_t)S(*a) << n);   // overflow: con extensión de signo
    *a = (int32_t)o;
    modificaCC(*a, o, c, mv);
}

void SHR(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{
    uint32_t n = (uint32_t)*b;
    *a = (n >= 32) ? 0 : (int32_t)((uint32_t)*a >> n);
    modificaCC(*a, *a, 0, mv);
}

void SAR(int32_t *a, int32_t *b, MaquinaVirtual *mv) 
{
    uint32_t n = (uint32_t)*b;
    *a = (n >= 32) ? (*a < 0 ? -1 : 0) : (*a >> n);
    modificaCC(*a, *a, 0, mv);
}

void LDH(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a = (int32_t)(((uint32_t)*a & 0x0000FFFF) | (((uint32_t)*b & 0x0000FFFF) << 16));
}

void LDL(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    *a = (int32_t)(((uint32_t)*a & 0xFFFF0000) | ((uint32_t)*b & 0x0000FFFF));
}

void RND(int32_t *a, int32_t *b, MaquinaVirtual *mv)
{
    int64_t tope = (int64_t)*b + 1;
    if (tope <= 0)
        *a = 0;
    else
        *a = (int32_t)((((uint64_t)rand() << 30) ^ ((uint64_t)rand() << 15) ^ rand()) % tope);
}
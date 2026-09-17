#include "main.h"
 
void error(int ce)
{
    if (ce == InstInv)
        printf("\nINSTRUCCION INVALIDA\n");
    else if (ce == DivCero)
        printf("\nDIVICION POR CERO\n");
    else if (ce == FalloSeg)
        printf("\nFALLO DE SEGMENTO\n");
    exit(1);
}


void decoOperacion(uint8_t operacion)
{
    uint8_t tam_opA,tam_opB;
    
    // los tamaños son tmbn el tipo de operando
    tam_opB=operacion >> 6;
    tam_opA=(operacion >> 4) & 0b0011;
    REGS[OP2]=((uint32_t)tam_opB) << 24; 
    REGS[OP1]=((uint32_t)tam_opA) << 24;
    
    REGS[OPC]=operacion & 0b00011111;
}

void infoOperando(int op, uint32_t dirFisOP) // se les pasa la constante OP1 u OP2 dependiendo de que registro se quiera utilizar,
{
    unsigned int tam_op=REGS[op] >> 24;
    uint32_t buffer=0;

    buffer=leeMem(dirFisOP,REGS[IP],tam_op); //trae de memoria la info del operando
    REGS[op]= (REGS[op] & 0xFF000000) | (buffer & 0x00FFFFFF); //carga el registro OP1 u OP2 (segun corresponda) con la info en en buffer
}

int32_t valorOperado(int op) // se les pasa la constante OP1 u OP2 dependiendo de que registro se quiera utilizar,
{
    int tipo_op=REGS[op] >> 24;
    switch (tipo_op)
    {
        case 1: //registro
                int cod_reg=REGS[op] & 0x000000FF;
                return REGS[cod_reg];
            break;
        case 2: //inmediato
                int16_t valor=REGS[op] & 0x0000FFFF;
                return (int32_t) valor;
            break;
        case 3: //memoria
                int16_t offset=REGS[op] >> 8;
                uint8_t puntero=REGS[op] & 0x000000FF;
                uint32_t dirFis=CalculaDirFis(puntero,offset);
                return leeMem(dirFis,puntero,4);
            break;
        default:
            break;
    }
}

int32_t leeMem(uint32_t dirFis, uint32_t puntero, int cant) //cant no debe pasar de 4, dado que leeMem devuelve 34 bytes en todos los casos, la funcion que la llame debe saber cuantos bytes va a usar
{
    int32_t buffer=0;
    int i;
    uint8_t aux;

    for (i=0; i<cant; i++)
    {
        if (dirValida(dirFis+i,puntero))
        {
            aux=RAM[dirFis+i];
            buffer= (buffer << 8) | aux;
        }
    }
    return buffer;
}

void guardaMem(uint32_t dirFis, uint32_t puntero, int cant, uint32_t valor) 
{
    int i;
    uint8_t aux;

    for (i = 0; i < cant; i++) 
    {
        if (dirValida(dirFis + i, puntero)) 
        {
            // Calculamos cuánto debemos desplazar los bits a la derecha
            // Para el primer byte (i=0), desplazamos más; para el último (i=cant-1), no desplazamos.
            int shift = (cant - 1 - i) * 8;
            
            // Extraemos el byte correspondiente aplicando una máscara
            aux = (valor >> shift) & 0xFF;
            
            // Guardamos el byte en la memoria RAM
            RAM[dirFis + i] = aux;
        }
    }
}

uint32_t calculaDirFis(uint32_t puntero, int16_t offset)
{
    
}
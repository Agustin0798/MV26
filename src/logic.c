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

void infoOperando(int op)
{
    unsigned int tam_op=REGS[op] >> 24;
    switch (tam_op)
    {
        case 1: //registro
                
            break;
        
        default:
            break;
    }
}

int32_t valorOperado(int op)
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
                
            break;
        default:
            break;
    }
}
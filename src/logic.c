#include "mv.h"
 
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


void decoOperacion(uint8_t operacion, MaquinaVirtual *mv) //decodifica la operacion y carga los registros OP1, OP2 y OPC
{
    uint8_t tam_opA,tam_opB;
    
    // los tamaños son tmbn el tipo de operando
    tam_opB=operacion >> 6;
    tam_opA=(operacion >> 4) & 0b0011;
    mv->REGS[OP2]=((uint32_t)tam_opB) << 24; 
    mv->REGS[OP1]=((uint32_t)tam_opA) << 24;
    
    mv->REGS[OPC]=operacion & 0b00011111;
}

void infoOperando(int op, uint32_t dirFisOP, MaquinaVirtual *mv) // se les pasa la constante OP1 u OP2 dependiendo de que registro se quiera utilizar,
{
    unsigned int tam_op=mv->REGS[op] >> 24;
    uint32_t buffer=0;

    buffer=leeMem(dirFisOP,mv->REGS[IP],tam_op,mv); //trae de memoria la info del operando
    mv->REGS[op]= (mv->REGS[op] & 0xFF000000) | (buffer & 0x00FFFFFF); //carga el registro OP1 u OP2 (segun corresponda) con la info en en buffer
}

int32_t valorOperado(int op, MaquinaVirtual *mv) // se les pasa la constante OP1 u OP2 dependiendo de que registro se quiera utilizar,
{
    int tipo_op=mv->REGS[op] >> 24;
    switch (tipo_op)
    {
        case 1: //registro
                int cod_reg=mv->REGS[op] & 0x000000FF;
                return mv->REGS[cod_reg];
            break;
        case 2: //inmediato
                int16_t valor=mv->REGS[op] & 0x0000FFFF;
                return (int32_t) valor;
            break;
        case 3: //memoria
                int16_t offset=mv->REGS[op] >> 8;
                uint8_t puntero=mv->REGS[op] & 0x000000FF;
                uint32_t dirFis=CalculaDirFis(puntero,offset,mv);
                return leeMem(dirFis,puntero,4,mv);
            break;
        default:
            break;
    }
}

int32_t leeMem(uint32_t dirFis, uint32_t puntero, int cant, MaquinaVirtual *mv) //cant no debe pasar de 4, dado que leeMem devuelve 34 bytes en todos los casos, la funcion que la llame debe saber cuantos bytes va a usar
{
    int32_t buffer=0;
    int i;
    uint8_t aux;

    for (i=0; i<cant; i++)
    {
        if (dirValida(dirFis+i,puntero,mv))
        {
            aux=mv->RAM[dirFis+i];
            buffer= (buffer << 8) | aux;
        }
    }
    return buffer;
}

void guardaMem(uint32_t dirFis, uint32_t puntero, int cant, uint32_t valor, MaquinaVirtual *mv) 
{
    int i;
    uint8_t aux;

    for (i = 0; i < cant; i++) 
    {
        if (dirValida(dirFis + i, puntero, mv)) 
        {
            // Calculamos cuánto debemos desplazar los bits a la derecha
            // Para el primer byte (i=0), desplazamos más; para el último (i=cant-1), no desplazamos.
            int shift = (cant - 1 - i) * 8;
            
            // Extraemos el byte correspondiente aplicando una máscara
            aux = (valor >> shift) & 0xFF;
            
            // Guardamos el byte en la memoria RAM
            mv->RAM[dirFis + i] = aux;
        }
    }
}

uint32_t calculaDirFis(uint32_t puntero, int16_t offset, MaquinaVirtual *mv)
{
    uint16_t pos_tds=puntero >> 16;
    int16_t offset_p=puntero & 0x0000FFFF;
    uint32_t dirFis;
    uint32_t base_seg=mv->TDS[pos_tds] >> 16;  
    
    dirFis=base_seg+ (int32_t) offset+ (int32_t) offset_p;

    return dirFis;
}

int dirValida(uint32_t dirFis, uint32_t puntero, MaquinaVirtual *mv)
{
    uint16_t pos_tds=puntero >> 16;
    uint32_t base_seg=mv->TDS[pos_tds] >> 16;
    uint32_t tam_seg=mv->TDS[pos_tds] & 0x0000FFFF;
    uint32_t fin_seg=base_seg+tam_seg;

    if ((dirFis >=base_seg) && (dirFis < fin_seg))
        return 1;
    else
        error(FalloSeg);

}
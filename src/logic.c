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

void leeOperacion(uint8_t *operacion, MaquinaVirtual *mv) //lee el primer byte de la instruccion (tipos de operando y cod de operacion) 
{
    uint8_t operacion;
    int16_t dirFisIP;

    mv->REGS[OP2]=0;
    mv->REGS[OP1]=0;
    dirFisIP=calculaDirFis(0,mv->REGS[IP],mv);
    dirValida(mv->REGS[IP],dirFisIP,mv);
    (*operacion)=mv->RAM[dirFisIP]; 
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

void infoOperando(MaquinaVirtual *mv)
{
    int16_t dirFis;
    int i;
    uint32_t buffer;
    uint8_t aux;
    uint8_t tam_B,tam_A;

    if (mv->REGS[OP2] != 0) //puedo tener al menos un operando?
    {
        buffer=0;
        tam_B=mv->REGS[OP2] >> 24;
        dirFis=calculaDirFis(1,mv->REGS[IP],mv);
        for (i=0; i<tam_B; i++)
        {
            aux=mv->RAM[dirFis+i];
            buffer= (buffer << 8) | aux;
        }
        mv->REGS[OP2]= (mv->REGS[OP2] & 0xFF000000) | (buffer & 0x00FFFFFF); // carga el OP2 con el tamaño y la info del operando

        if (mv->REGS[OP1] != 0) // tengo dos operandos?
        {
            buffer=0;
            tam_A=mv->REGS[OP1] >> 24;
            dirFis=calculaDirFis(1+tam_B,mv->REGS[IP],mv);
            for (i=0; i<tam_A; i++)
            {
                aux=mv->RAM[dirFis+i];
                buffer= (buffer << 8) | aux;
            }
            mv->REGS[OP1]= (mv->REGS[OP1] & 0xFF000000) | (buffer & 0x00FFFFFF); // carga el OP1 con el tamaño y la info del operando
        }
    }
}

int32_t valorOperado(int op, MaquinaVirtual *mv) // se les pasa la constante OP1 u OP2 dependiendo de que registro se quiera utilizar,
{
    int tipo_op=mv->REGS[op] >> 24;
    int i;

    switch (tipo_op)
    {
        case 1: //registro
                uint8_t cod_reg=mv->REGS[op] & 0x000000FF;
                return mv->REGS[cod_reg];
            break;
        case 2: //inmediato
                int16_t valor=mv->REGS[op] & 0x0000FFFF;
                return (int32_t) valor;
            break;
        case 3: //memoria
                int16_t offset=mv->REGS[op] >> 8;
                uint8_t cod_reg=mv->REGS[op] & 0x000000FF;
                uint32_t puntero=mv->REGS[cod_reg];
                int16_t dirFis=calculaDirFis(offset,puntero,mv);
                mv->REGS[LAR]=puntero;
                mv->REGS[MAR]= (4 << 16) | dirFis;
                leeMem(mv);
                return mv->REGS[MBR];
            break;
        default:
            break;
    }
}

void leeMem(MaquinaVirtual *mv) //El mar no debe pedir mas de 4 bytes, dado que el MBR es de 4 bytes. El dato se guarda en el MBR directamente
{
    int32_t buffer=0;
    int i;
    uint8_t aux;
    unsigned int cant=mv->REGS[MAR] >> 16;
    uint16_t dirFis=mv->REGS[MAR] & 0x0000FFFF;

    for (i=0; i<cant; i++)
    {
        dirValida(mv->REGS[LAR],dirFis+i,mv);
        aux=mv->RAM[dirFis+i];
        buffer= (buffer << 8) | aux;
    }

    mv->REGS[MBR]=buffer;
}

void guardaMem(MaquinaVirtual *mv) 
{
    int i;
    uint8_t aux;
    unsigned int cant=mv->REGS[MAR] >> 16;
    uint16_t dirFis=mv->REGS[MAR] & 0x0000FFFF;
    uint32_t valor=mv->REGS[MBR];
    uint32_t puntero=mv->REGS[LAR];
    int shift;

    for (i = 0; i < cant; i++) 
    {
        dirValida(puntero,dirFis+i,mv);
        // Calculo cuanto debo desplazar los bits a la derecha
        shift= (cant - 1 - i) * 8;
        // Extraemos el byte correspondiente aplicando una máscara
        aux= (valor >> shift) & 0xFF;
            
        // Guardamos el byte en la memoria RAM
        mv->RAM[dirFis + i] = aux;
    }
}

int16_t calculaDirFis(int16_t offset, uint32_t puntero, MaquinaVirtual *mv) //guarda la dirFis ya en el MAR
{
    uint16_t pos_tds=puntero >> 16;
    int16_t offset_p=puntero & 0x0000FFFF;
    int16_t dirFis;
    int16_t base_seg=mv->TDS[pos_tds] >> 16;  
    
    dirFis=base_seg+ offset+ offset_p;

    return dirFis;
}

void dirValida(uint32_t puntero, int16_t dirFis, MaquinaVirtual *mv)
{
    uint16_t pos_tds=puntero >> 16;
    int16_t base_seg=mv->TDS[pos_tds] >> 16;
    uint16_t tam_seg=mv->TDS[pos_tds] & 0x0000FFFF;
    int16_t fin_seg=base_seg+tam_seg;

    if ((dirFis < base_seg) || (dirFis >= fin_seg))
        error(FalloSeg);
}

void decideGuardar(uint32_t valor1, uint32_t valor2, MaquinaVirtual *mv)
{
    if (((mv->REGS[OPC] >= 0x10) || (mv->REGS[OPC] <= 0x1F)) && (mv->REGS[OPC] != 0x15)) //La operacion es de dos operandos y no es el cmp?
    {
        guardaOP(OP1,valor1,mv);
        if (mv->REGS[OPC] == 0x19) //La operacion es el swap?
            guardaOP(OP2,valor2,mv);
    } 
    else if (mv->REGS[OPC] == 0x0A) //La operacion es el not?
    {
        guardaOP(OP2,valor2,mv);
    }
}

void guardaOP(uint8_t op, uint32_t valor, MaquinaVirtual *mv)
{
    uint8_t tipo_op=mv->REGS[op] >> 24;

    switch (tipo_op)
    {
        case 1: //registro
                uint8_t cod_reg=mv->REGS[op] & 0x000000FF;
                mv->REGS[cod_reg]=valor;
            break;
        case 2: //inmediato
            break;
        case 3: //memoria
                int16_t offset=mv->REGS[op] >> 8;
                uint8_t cod_reg=mv->REGS[op] & 0x000000FF;
                uint32_t puntero=mv->REGS[cod_reg];
                int16_t dirFis=calculaDirFis(offset,puntero,mv);
                mv->REGS[LAR]=puntero;
                mv->REGS[MAR]= (4 << 16) | dirFis;
                mv->REGS[MBR]=valor;
                guardaMem(mv);
            break;
        default:
            break;
    }
}
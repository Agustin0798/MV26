#include "main.h"

void (*func[MF])(int32_t *,int32_t *, MaquinaVirtual *mv) = {
    SYS,   // 00
    JMP,   // 01
    JP,    // 02
    JN,    // 03
    JZ,    // 04
    JC,    // 05
    JV,    // 06
    JNP,   // 07
    JNN,   // 08
    JNZ,   // 09
    NOT,   // 0A
    NULA,  // 0B 
    NULA,  // 0C 
    NULA,  // 0D 
    NULA,  // 0E 
    STOP,  // 0F
    MOV,   // 10
    ADD,   // 11
    SUB,   // 12
    MUL,   // 13
    DIV,   // 14
    CMP,   // 15
    AND,   // 16
    OR,    // 17
    XOR,   // 18
    SWAP,  // 19
    SHL,   // 1A
    SHR,   // 1B
    SAR,   // 1C
    LDL,   // 1D
    LDH,   // 1E
    RND    // 1F
};


void Ejecutar(MaquinaVirtual *mv)
{
    uint8_t operacion,cod_op,t_op1,t_op2;
    int32_t valor1,valor2;
    int16_t dirFis_IP=calculaDirFis(0,mv->REGS[IP],mv);


    while (dirFis_IP > -1)
    {
        printf("EJECUTAR %d\n",dirFis_IP);
        dirValida(mv->REGS[IP],dirFis_IP,mv);
        valor1=0;
        valor2=0;
        cod_op=0;
        t_op1=0;
        t_op2=0;
        leeOperacion(&operacion,mv); //lee el byte que posee los codigos de operando y opeacion
        decoOperacion(operacion,mv); //decodifica los codigos de operando y operacion y los guarda en los registros pertinentes
        infoOperando(mv); //extrae los bytes que poseen la informacion de los operandos
        valor2=valorOperado(OP2,mv);
        valor1=valorOperado(OP1,mv);
        cod_op=mv->REGS[OPC];
        printf("cod op %x\n",cod_op);
        t_op1=mv->REGS[OP1] >> 24;
        t_op2=mv->REGS[OP2] >> 24;
        mv->REGS[IP]=mv->REGS[IP] + 1 + t_op1 + t_op2;
        printf("IP: %d top1: %d top2:%d \n",mv->REGS[IP],t_op1,t_op2);
        func[OPC](&valor1,&valor2,mv);
        decideGuardar(valor1,valor2,mv);
        dirFis_IP=calculaDirFis(0,mv->REGS[IP],mv);
    }

}
#include "main.h"

const char * Mnemonicos[32]={
    "SYS",   // 00
    "JMP",   // 01
    "JP",    // 02
    "JN",    // 03
    "JZ",    // 04
    "JC",    // 05
    "JV",    // 06
    "JNP",   // 07
    "JNN",   // 08
    "JNZ",   // 09
    "NOT",   // 0A
    "PUSH",  // 0B 
    "POP",  // 0C 
    "CALL",  // 0D 
    "RET",  // 0E 
    "STOP",  // 0F
    "MOV",   // 10
    "ADD",   // 11
    "SUB",   // 12
    "MUL",   // 13
    "DIV",   // 14
    "CMP",   // 15
    "AND",   // 16
    "OR",    // 17
    "XOR",   // 18
    "SWAP",  // 19
    "SHL",   // 1A
    "SHR",   // 1B
    "SAR",   // 1C
    "LDL",   // 1D
    "LDH",   // 1E
    "RND"    // 1F
};

const char *NomReg[32] = {
    "IP",  "OPC", "OP1", "OP2",   // 0-3
    "LAR", "MAR", "MBR",          // 4-6
    "SP",  "BP",  "",             // 7-9 (9 reservado)
    "EAX", "EBX", "ECX", "EDX", "EEX", "EFX",  // 10-15
    "AC",  "CC",                  // 16-17
    "", "", "", "", "", "", "", "",  // 18-25 (reservados)
    "CS",  "DS",  "ES",  "SS",  "KS",  "PS"    // 26-31
};

void disassembler(MaquinaVirtual mv)
{   
    uint16_t posTDS,tamSeg,baseSeg,dirFis,offset;
    uint8_t operacion,tamA,tamB,opc,i;

    if (mv.REGS[KS] != -1)
    {
        posTDS=(mv.REGS[KS] >> 16) & 0x0000FFFF;
        baseSeg=(mv.TDS[posTDS] >> 16) & 0x0000FFFF;
        tamSeg=mv.TDS[posTDS] & 0x0000FFFF;
        dirFis=baseSeg;
        offset=0;
        while (offset < tamSeg)
        {
            dirFis+=offset;
            printf(" [%04X]",dirFis);           //promp de direccion fisica
            operacion=mv.RAM[dirFis];
            printf(" %02X",operacion);    //instruccion (cods ops y opc)
            opc=operacion & 0x1F;
            tamA=(operacion >> 4) & 0b0011;
            tamB=operacion >> 6;
            for (i=0; i < tamA+tamB; i++)
                printf(" %02X",mv.RAM[dirFis+1+i]);  //info de los operandos
            
        }
    }
}
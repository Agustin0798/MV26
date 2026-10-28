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

static void nombreRegistro(uint8_t cod, uint8_t sector, char *out)
{
    char l;

    if (cod >= 10 && cod <= 15)                 // EAX..EFX con pseudónimo
    {
        l = 'A' + (cod - 10);
        switch (sector)
        {
            case 0:  sprintf(out, "E%cX", l); break;   // 4 bytes
            case 1:  sprintf(out, "%cL",  l); break;   // 4to byte
            case 2:  sprintf(out, "%cH",  l); break;   // 3er byte
            default: sprintf(out, "%cX",  l); break;   // 2 bytes
        }
    }
    else
        strcpy(out, NomReg[cod]);
}

// Decodifica el operando que empieza en ram[dir] y deja el texto en out
static void textoOperando(uint8_t *ram, uint16_t dir, uint8_t tipo, char *out)
{
    char reg[8];
    int16_t valor;
    uint8_t b, tam;
    const char *pref;

    out[0] = '\0';
    switch (tipo)
    {
        case 1:                                   // registro (1 byte)
            b = ram[dir];
            nombreRegistro(b & 0x1F, (b >> 5) & 0x03, out);
            break;

        case 2:                                   // inmediato (2 bytes, big endian)
            valor = (int16_t)((ram[dir] << 8) | ram[dir + 1]);
            sprintf(out, "%d", valor);
            break;

        case 3:                                   // memoria (3 bytes): offset(2) + tamaño/registro(1)
            valor = (int16_t)((ram[dir] << 8) | ram[dir + 1]);
            b     = ram[dir + 2];
            tam   = (b >> 5) & 0x07;
            nombreRegistro(b & 0x1F, 0, reg);
            switch (tam)
            {
                case 1:  pref = "b"; break;
                case 2:  pref = "w"; break;
                case 4:  pref = "l"; break;
                default: pref = "";  break;
            }
            sprintf(out, "%s[%s%+d]", pref, reg, valor);
            break;
    }
}


void disassembler(MaquinaVirtual mv)
{   
    uint16_t posTDS,tamSeg,baseSeg,dirFis,offset,entry;
    uint8_t operacion,tamA,tamB,opc,i,aux,len;
    char opA[32], opB[32];
    int vercion= mv.vercion;

    if ((vercion == 2) && (mv.REGS[KS] != -1))  //Existe el KS
    {
        posTDS=(mv.REGS[KS] >> 16) & 0x0000FFFF;
        baseSeg=(mv.TDS[posTDS] >> 16) & 0x0000FFFF;
        tamSeg=mv.TDS[posTDS] & 0x0000FFFF;
        dirFis=baseSeg;
        offset=0;
        while (offset < tamSeg)
        {
            dirFis+=offset;
            printf(" [%04X] ",dirFis);           //promp de direccion fisica
            
            len=1;
            while (mv.RAM[dirFis + len - 1] != '\0') //longitud del string a mostrar
                len++;

            if (len > 7) //El string supera 7 bytes de longitud?
            {
                for (i=0; i < 6; i++)
                    printf("%02X ", mv.RAM[dirFis + i]);
                printf(".. ");
            }
            else
            {
                for (i = 0; i < len; i++)
                    printf("%02X ", mv.RAM[dirFis + i]);
                for (i=len; i < 7; i++) //Relleno con espacios vacios hasta llegar a 7
                    printf("   ");
            }

            printf("| \"");
            for (i = 0; i < len - 1; i++) //Muestro el estring
            {
                aux = mv.RAM[dirFis + i];
                printf("%c", (aux >= 32 && aux <= 126) ? aux : '.');
            }
            printf("\"\n");

            offset += len;
            
        }
    }

    posTDS=(mv.REGS[CS] >> 16) & 0x0000FFFF;
    baseSeg=(mv.TDS[posTDS] >> 16) & 0x0000FFFF;
    tamSeg=mv.TDS[posTDS] & 0x0000FFFF;
    dirFis=baseSeg;
    offset=0;
    entry=calculaDirFis(0,mv.REGS[IP],&mv);
    while (offset < tamSeg)
    {
        tamB=0;
        tamA=0;
        opc=0;
        dirFis+=offset;
        if ((vercion == 2) && (dirFis == entry))   //Si estoy en vercion dos veo si la direccion fisica actual es el enty point
            printf(">");
        else
            printf(" ");
        printf("[%04X] ",dirFis);  
        
        operacion=mv.RAM[dirFis];
        printf("%02X ",operacion); //Muestro el byte que indica tamaño/tipo de operandos y codigo de operacion

        opc=operacion & 0b00011111;

        if (opc >= 0x10)                               // dos operandos
        {
            tamB = (operacion >> 6) & 0b0011;
            tamA = (operacion >> 4) & 0b0011;
        }
        else if (opc == 0x0E || opc == 0x0F)           // RET, STOP
        {
            tamA = 0;
            tamB = 0;
        }
        else                                           // un operando: el tipo está en los 2 bits altos
        {
            tamA = (operacion >> 6) & 0b0011;
            tamB = 0;
        }

        for (i=0; i < (tamA + tamB); i++)   //Muestro los bytes de informacion de los operandos
            printf("%02X ",mv.RAM[dirFis+1+i]);
        for (; i < 6; i++)
            printf("   ");

        printf("| ");

        printf("%s ",Mnemonicos[opc]);
        

        textoOperando(mv.RAM, dirFis + 1,tamB, opB);
        textoOperando(mv.RAM, dirFis + 1 + tamB, tamA, opA);

        printf("%s", Mnemonicos[opc]);
        if (tamA != 0)
            printf(" %s", opA);
        if (tamB != 0)
            printf(", %s", opB);
        printf("\n");

        offset += 1 + tamA + tamB;
    }
}
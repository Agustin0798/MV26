#include "mv.h"

void (*func[MF])(int32_t *,int32_t *) = {
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

void Text_Assembler(char *buffer, uint32_t info, uint8_t t_op)
{
    const char * const REGISTROS[32] = {
        "IP",   // 0: Instrucción
        "OPC",  // 1: Instrucción
        "OP1",  // 2: Instrucción
        "OP2",  // 3: Instrucción
        "LAR",  // 4: Acceso a memoria
        "MAR",  // 5: Acceso a memoria
        "MBR",  // 6: Acceso a memoria
        "R_INX",  // 7: Reservado
        "R_INX",  // 8: Reservado
        "R_INX",  // 9: Reservado
        "EAX",  // 10: Registros de propósito general
        "EBX",  // 11: Registros de propósito general
        "ECX",  // 12: Registros de propósito general
        "EDX",  // 13: Registros de propósito general
        "EEX",  // 14: Registros de propósito general
        "EFX",  // 15: Registros de propósito general
        "AC",   // 16: Acumulador
        "CC",   // 17: Código de condición
        "R_INX",  // 18: Reservado
        "R_INX",  // 19: Reservado
        "R_INX",  // 20: Reservado
        "R_INX",  // 21: Reservado
        "R_INX",  // 22: Reservado
        "R_INX",  // 23: Reservado
        "R_INX",  // 24: Reservado
        "R_INX",  // 25: Reservado
        "CS",   // 26: Segmentos
        "DS",   // 27: Segmentos
        "R_INX",  // 28: Reservado
        "R_INX",  // 29: Reservado
        "R_INX",  // 30: Reservado
        "R_INX"   // 31: Reservado
    };
    switch (t_op)
    {
        case 0x01: //registro
                unsigned char indx= info & 0x1F;
                strcpy(buffer,REGISTROS[indx]);
                strcat(buffer,'\0');
            break;
        case 0x02: //inmediato
                info= info & 0x0000FFFF;
                sprintf(buffer,"%d",(int16_t) info);
            break;
        case 0x03: //memoria
                unsigned char indx= info & 0x1F;
                int16_t offset= (int16_t) (info & 0x0000FFFF);

                if (offset == 0)
                    sprintf(buffer,"[%s]",REGISTROS[indx]);
                else if (offset > 0)
                    sprintf(buffer,"[%s+%d]",REGISTROS[indx],offset);
                else
                    sprintf(buffer,"[%s%d]",REGISTROS[indx],offset);
            break;
        default:
            break;
    }
}

void DisAssembler(MaquinaVirtual mv)
{
    uint16_t Fin, act, inic, tam;
    uint32_t puntero;
    uint32_t info1,info2;
    const char *Mnom[]={
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
        "NULA",  // 0B 
        "NULA",  // 0C 
        "NULA",  // 0D 
        "NULA",  // 0E 
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
    uint8_t byte,t_op1,t_op2,cod_op,aux;
    char str1[40],str2[40];
    int i;

    puntero=mv.REGS[IP];
    act=calculaDirFis(0,puntero,&mv);
    tam=mv.REGS[CC] & 0x0000FFFF;
    Fin=calculaDirFis(tam,puntero,&mv);
    while (act < Fin)
    {
        t_op1=t_op2=0;
        info1=info2=0;
        strcpy(str1,"");
        strcpy(str2,"");

        byte=mv.RAM[act];
        cod_op=byte & 0x1F;

        t_op2=(byte >> 6) & 0x03;
        t_op1=(byte >> 4) & 0x03;
        if (t_op2 > 0) //La tiene operandos?
        {
            for (i=0; i<t_op2; i++)
            {
                aux=mv.RAM[act+1+i];
                info2= (info2 << 8) | aux;
            }
            Text_Assembler(str2,info2,t_op2);
            if (t_op1 > 0)
            {
                for (i=0; i<t_op1; i++)
                {
                    aux=mv.RAM[act+1+t_op2+i];
                    info2= (info2 << 8) | aux;
                }
                Text_Assembler(str1,info1,t_op1);
            }
        }

        printf(" [%04X] ",act);
        
        for (i=0; i< (t_op2 + t_op1 + 1); i++)
            printf("%02X ",mv.RAM[act+i]);
        if (i < 7)
            for (i; i< 8; i++)
                printf("  ");
        printf("| ");

        printf("%-4s ",Mnom[cod_op]);
        if (t_op2 > 0)
            if (t_op1 > 0)
                printf("%s, %s",str1,str2);
            else
                printf("%s",str2);
        printf("\n");
    }
}
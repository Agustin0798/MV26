#include "main.h"

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

int8_t RAM[MM];
int32_t REGS[MR];
int32_t TDS[MTDS];

int main(char *argv[],int argc)
{
    return 0;
}
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

int8_t RAM[MM];
int32_t REGS[MR];
int32_t TDS[MTDS];

static void uso(const char *prog) {
    fprintf(stderr, "Uso: %s <programa.vmx> [-d]\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        uso(argv[0]);
        return EXIT_FAILURE;
    }

    const char *programa = NULL;
    int modo_disassembler = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            modo_disassembler = 1;
        } else if (argv[i][0] != '-') {
            programa = argv[i];
        } else {
            fprintf(stderr, "Opción desconocida: %s\n", argv[i]);
            uso(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (!programa) {
        uso(argv[0]);
        return EXIT_FAILURE;
    }

    if (modo_disassembler) {
        printf("mv1vm: TODO implementar disassembler de '%s'\n", programa);
    }
    printf("mv1vm: TODO implementar ejecución de '%s'\n", programa);

    return EXIT_SUCCESS;
}
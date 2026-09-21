#include "mv.h"



static void uso(const char *prog) {
    fprintf(stderr, "Uso: %s <programa.vmx> [-d]\n", prog);
}

int main(int argc, char *argv[]) {

    MaquinaVirtual mv;

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
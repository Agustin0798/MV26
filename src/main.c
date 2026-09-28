#include "main.h"


static void uso(const char *prog) {
    fprintf(stderr, "Uso: %s <programa.vmx> [-d]\n", prog);
}

int main(int argc, char *argv[]) {

    MaquinaVirtual mv;
    srand(time(NULL));

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
    
    cargar_programa(programa, &mv);
    if (modo_disassembler) {
       disassembler(mv.RAM, 0, mv.TDS[0] & 0x0000FFFF);
    }
    Ejecutar(&mv);


    return EXIT_SUCCESS;
}



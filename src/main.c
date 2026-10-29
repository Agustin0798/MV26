#include "main.h"
static int termina_en(const char *s, const char *ext) {
    size_t ls = strlen(s), le = strlen(ext);
    return ls >= le && strcmp(s + ls - le, ext) == 0;
}

static void uso(const char *prog) {
    fprintf(stderr, "Uso: %s <programa.vmx> [-d]\n", prog);
}

//vmx [f.vmx] [f.vmi] [m=M] [-d] [-p p1 ... pN]
int main(int argc, char *argv[]) {

    MaquinaVirtual mv;
    srand(time(NULL));

    if (argc < 2) {
        uso(argv[0]);
        return EXIT_FAILURE;
    }

    char *programa = NULL;   // .vmx
    mv.imagen = NULL;     // .vmi
    int memoria = 16;              // KiB, por defecto 16
    int modo_disassembler = 0;     // se activa si se pasa el flag -d
    char **params = NULL;    // primer parámetro dentro de argv
    int cant_params = 0;           // cantidad de parámetros ingresados

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            modo_disassembler = 1;
        } 
        else if (strncmp(argv[i], "m=", 2) == 0) {
            memoria = atoi(argv[i] + 2);

        } 
        else if (termina_en(argv[i], ".vmx")) {
            programa = argv[i];
        } 
        else if (termina_en(argv[i], ".vmi")) {
            mv.imagen = argv[i];
        } 
        else if (strcmp(argv[i], "-p") == 0) {
            // -p va siempre al final: lo que sigue son parámetros
            params = (char **)&argv[i + 1];
            cant_params = argc - i - 1;
            break;
        }
    }

    mv.tamRam = memoria * 1024; // Convertir KiB a bytes
    mv.RAM = malloc(mv.tamRam);

    // Sin .vmx se ignoran los parámetros
    if (!programa) {
        params = NULL;
        cant_params = 0;
    }
    
    // Para hacer: usar imagen, memoria, params y cant_params

    cargar_programa(programa, &mv,cant_params,params);
    if (modo_disassembler) {
        disassembler(mv);
    }
    printf("\nIniciando ejecucion de programa...\n\n");
    Ejecutar(&mv);
    printf("\nEjecucion finalizada.\n");

    return EXIT_SUCCESS;
}



#include <main.h>

void cargar_imagen(const char* nombre_archivo, MaquinaVirtual* mv) {
    FILE* archivo = fopen(nombre_archivo, "rb");
    if (!archivo) {
        fprintf(stderr, "Error al abrir el archivo de imagen: %s\n", nombre_archivo);
        exit(EXIT_FAILURE);
    }

    //leer identificador y version
    uint8_t id[5];
    if (fread(id, 1, 5, archivo) != 5) {
        fprintf(stderr, "Error al leer el identificador de la imagen: %s\n", nombre_archivo);
        fclose(archivo);
        exit(EXIT_FAILURE);
    }

    // Leer la imagen y cargarla en la memoria de la máquina virtual
    if (fread(mv->RAM, 1, mv->tamRam, archivo) != mv->tamRam) {
        fprintf(stderr, "Error al leer la imagen: %s\n", nombre_archivo);
        fclose(archivo);
        exit(EXIT_FAILURE);
    }

    

    fclose(archivo);
}
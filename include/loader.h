#pragma once

#include "mv.h"

// Carga en memoria el programa .vmx pasado por parametro, inicializando la TDS y los registros de la máquina virtual.
void cargar_programa(const char *path, MaquinaVirtual *mv);

void inicializar_tds(MaquinaVirtual *mv, int version, uint16_t tam_codigo);
void inicializar_registros(MaquinaVirtual *mv, int version);
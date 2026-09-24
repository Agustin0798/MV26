#pragma once

#include "mv.h"

const char *obtener_mnemonico(unsigned char opcode);
int desensamblar_instruccion(const unsigned char *instr, unsigned short direccion_fisica, char *linea, size_t tam_linea);

void disassembler(const unsigned char *memoria, unsigned short base_cs, unsigned short tam_codigo);
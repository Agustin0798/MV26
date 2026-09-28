#include "main.h"

/* Maximo de bytes que puede tener una instruccion:
 * 1 (header) + 3 (operando memoria) + 3 (operando memoria) = 7 */
#define MAX_BYTES_INSTR 7

/* Ancho fijo de la columna hexadecimal en la salida, para que el '|'
 * quede alineado sin importar el tamano real de la instruccion.
 * Cada byte ocupa "XX " (3 caracteres). */
#define ANCHO_COL_HEX (MAX_BYTES_INSTR * 3)

/* ------------------------------------------------------------------------
 * Nombre de registro (seccion 7). Una unica funcion con switch y strings
 * hardcodeados, sin generar tablas ni reejecutar logica de la VM.
 * ------------------------------------------------------------------------ */
static const char *nombre_registro(unsigned char codigo)
{
    switch (codigo) {
        case 0:  return "IP";
        case 1:  return "OPC";
        case 2:  return "OP1";
        case 3:  return "OP2";
        case 4:  return "LAR";
        case 5:  return "MAR";
        case 6:  return "MBR";
        case 10: return "EAX";
        case 11: return "EBX";
        case 12: return "ECX";
        case 13: return "EDX";
        case 14: return "EEX";
        case 15: return "EFX";
        case 16: return "AC";
        case 17: return "CC";
        case 26: return "CS";
        case 27: return "DS";
        default: return "RES"; /* registros reservados (7-9, 18-25, 28-31) */
    }
}

// Mnemonico de la instruccion 
const char *obtener_mnemonico(unsigned char opcode)
{
    switch (opcode) {
        /* --- dos operandos (0x10-0x1F) --- */
        case 0x10: return "MOV";
        case 0x11: return "ADD";
        case 0x12: return "SUB";
        case 0x13: return "MUL";
        case 0x14: return "DIV";
        case 0x15: return "CMP";
        case 0x16: return "AND";
        case 0x17: return "OR";
        case 0x18: return "XOR";
        case 0x19: return "SWAP";
        case 0x1A: return "SHL";
        case 0x1B: return "SHR";
        case 0x1C: return "SAR";
        case 0x1D: return "LDL";
        case 0x1E: return "LDH";
        case 0x1F: return "RND";

        /* --- un operando (0x00-0x0A) --- */
        case 0x00: return "SYS";
        case 0x01: return "JMP";
        case 0x02: return "JP";
        case 0x03: return "JN";
        case 0x04: return "JZ";
        case 0x05: return "JC";
        case 0x06: return "JV";
        case 0x07: return "JNP";
        case 0x08: return "JNN";
        case 0x09: return "JNZ";
        case 0x0A: return "NOT";

        /* --- sin operandos --- */
        case 0x0F: return "STOP";

        default:   return "???";
    }
}

/* ------------------------------------------------------------------------
 * Formatea un operando ya decodificado (tipo + valores crudos) como texto,
 * en la sintaxis del Assembler original, con los valores numericos en
 * decimal
 * ------------------------------------------------------------------------ */
static void formatear_operando(char *destino, size_t tam_destino, unsigned char tipo, unsigned char val_reg, unsigned short val_inm, short val_disp)
{
    char buffer[32];

    destino[0] = '\0';

    switch (tipo) {
        case 0x0: /* NINGUNO */
            break;

        case 0x1: // REGISTRO
            strncat(destino, nombre_registro(val_reg), tam_destino - 1);
            break;

        case 0x2: // INMEDIATO
            snprintf(buffer, sizeof(buffer), "%u", val_inm);
            strncat(destino, buffer, tam_destino - 1);
            break;

        case 0x3: // MEMORIA
            strncat(destino, "[", tam_destino - 1);
            strncat(destino, nombre_registro(val_reg), tam_destino - 1);
            if (val_disp != 0) {
                snprintf(buffer, sizeof(buffer), "%c%d", (val_disp >= 0) ? '+' : '-', (val_disp >= 0) ? val_disp : -val_disp);
                strncat(destino, buffer, tam_destino - 1);
            }
            strncat(destino, "]", tam_destino - 1);
            break;

        default:
            strncat(destino, "?", tam_destino - 1);
            break;
    }
}

/*
 * Lee de 'bytes' (a partir de la posicion 0) el valor crudo de un operando
 * de tipo 'tipo' y completa val_reg / val_inm / val_disp segun corresponda.
 * Devuelve la cantidad de bytes consumidos.
 */
static int leer_operando(const unsigned char *bytes, unsigned char tipo,
                          unsigned char *val_reg, unsigned short *val_inm,
                          short *val_disp)
{
    unsigned short disp_bruto;
    unsigned char  reg_byte;

    switch (tipo) {
        case 0x1: // REGISTRO
            *val_reg = bytes[0];
            return 1;

        case 0x2: // INMEDIATO
            *val_inm = (unsigned short)((bytes[0] << 8) | bytes[1]);
            return 2;

        case 0x3:
            /* 2 bytes de desplazamiento (big-endian, con signo) +
             * 1 byte de registro (los 5 bits bajos son el codigo). */
            disp_bruto = (unsigned short)((bytes[0] << 8) | bytes[1]);
            *val_disp  = (short)disp_bruto;
            reg_byte   = bytes[2];
            *val_reg   = reg_byte & 0x1F;
            return 3;

        default:
            return 0;
    }
}

/* ------------------------------------------------------------------------
 * Desensambla una instruccion ubicada en 'instr' (apuntando al primer byte). 
 * Escribe la linea completa en 'linea' (ya formateada, sin salto de final) 
 * y devuelve la logitud en bytes de la instruccion.
 * ------------------------------------------------------------------------ */
int desensamblar_instruccion(const unsigned char *instr, unsigned short direccion_fisica, char *linea, size_t tam_linea)
{
    unsigned char header = instr[0];
    unsigned char tipoA  = (header >> 6) & 0x3; /* tipo del 1er byte codificado -> OP2 si hay 2 operandos, o el unico operando */
    unsigned char tipoB  = (header >> 4) & 0x3; /* tipo del 2do byte codificado -> OP1, solo si hay 2 operandos */
    unsigned char nibble = header & 0x0F;
    unsigned char opcode;
    int dos_operandos = (tipoA != 0x0) && (tipoB != 0x0);

    unsigned char  reg_a = 0, reg_b = 0;
    unsigned short inm_a = 0, inm_b = 0;
    short          disp_a = 0, disp_b = 0;

    char op_a_txt[40] = "";
    char op_b_txt[40] = "";
    char hex_bytes[ANCHO_COL_HEX + 1];
    char operandos[96];
    const char *mnem;
    int pos, i, longitud;

    if (dos_operandos) {
        opcode = (unsigned char)(0x10 + nibble);
    } else {
        opcode = nibble;
    }
    mnem = obtener_mnemonico(opcode);

    /* Los bytes de los operandos, en orden inverso al Assembler: primero
     * el que corresponde a tipoA (OP2 si hay dos, o el unico operando),
     * despues el que corresponde a tipoB (OP1, si existe). */

    pos = 1; /* offset dentro de instr[], salteando el header */

    if (dos_operandos) {

        /* tipoA = tipo de OP2 (operando B en la sintaxis Assembler) */
        pos += leer_operando(&instr[pos], tipoA, &reg_b, &inm_b, &disp_b);

        /* tipoB = tipo de OP1 (operando A en la sintaxis Assembler) */
        pos += leer_operando(&instr[pos], tipoB, &reg_a, &inm_a, &disp_a);

    } else if (tipoA != 0x0) {

        /* un unico operando -> es el "OP_A" del formato de salida */
        pos += leer_operando(&instr[pos], tipoA, &reg_a, &inm_a, &disp_a);
    }
    longitud = pos;

    /* --- Armado de la columna hexadecimal --- */
    hex_bytes[0] = '\0';

    for (i = 0; i < longitud; i++) {
        char byte_txt[4];
        snprintf(byte_txt, sizeof(byte_txt), "%02X ", instr[i]);
        strncat(hex_bytes, byte_txt, sizeof(hex_bytes) - strlen(hex_bytes) - 1);
    }

    /* completar con espacios hasta el ancho fijo de columna */
    while (strlen(hex_bytes) < ANCHO_COL_HEX) {
        strncat(hex_bytes, " ", sizeof(hex_bytes) - strlen(hex_bytes) - 1);
    }

    /* --- Armado de los operandos en texto --- */
    if (dos_operandos) {
        formatear_operando(op_a_txt, sizeof(op_a_txt), tipoB, reg_a, inm_a, disp_a);
        formatear_operando(op_b_txt, sizeof(op_b_txt), tipoA, reg_b, inm_b, disp_b);
    } else if (tipoA != 0x0) {
        formatear_operando(op_a_txt, sizeof(op_a_txt), tipoA, reg_a, inm_a, disp_a);
    }

    operandos[0] = '\0';
    if (op_a_txt[0] != '\0') {
        strncat(operandos, op_a_txt, sizeof(operandos) - strlen(operandos) - 1);
        if (op_b_txt[0] != '\0') {
            strncat(operandos, ", ", sizeof(operandos) - strlen(operandos) - 1);
            strncat(operandos, op_b_txt, sizeof(operandos) - strlen(operandos) - 1);
        }
    }

    /* --- Linea final: [0000] XX XX ... | MNEM OP_A, OP_B --- */
    snprintf(linea, tam_linea, "[%04X] %s| %s%s%s", direccion_fisica, hex_bytes, mnem, (operandos[0] != '\0') ? " " : "", operandos);

    return longitud;
}


void disassembler(const unsigned char *memoria, unsigned short base_cs, unsigned short tam_codigo)
{
    unsigned short offset = 0;
    char linea[160];

    while (offset < tam_codigo) {
        const unsigned char *instr = &memoria[base_cs + offset];
        int longitud = desensamblar_instruccion(instr, (unsigned short)(base_cs + offset), linea, sizeof(linea));

        printf("%s\n", linea);

        if (longitud <= 0) {
            break;
        }
        offset = (unsigned short)(offset + longitud);
    }

    printf("\n");
}
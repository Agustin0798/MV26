#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//constantes de cantidad
#define MM 16384
#define MR 32
#define MTDS 8
#define MF 32

//constantes de registros
#define IP  0
#define OPC 1
#define OP1 2
#define OP2 3
#define LAR 4
#define MAR 5
#define MBR 6

#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15
#define AC  16
#define CC  17


#define CS  26
#define DS  27

/* ---------- Estado completo de la máquina virtual ---------- */

typedef struct {
    uint8_t RAM[MM];
    uint32_t TDS[MTDS];
    uint32_t REGS[MR];
} MaquinaVirtual;


//definiciones de variables globales
extern void (*func[MF])(int32_t *,int32_t *, MaquinaVirtual *mv);


/* -Helpers de empaquetado de 32 bits -
 * Varios campos de la especificación (entradas de la TDS, direcciones
 * lógicas, OP1/OP2, etc.) son un entero de 32 bits compuesto por dos
 * mitades de 16 bits: la mitad alta y la mitad baja.
 */

static inline int32_t empaquetar32(uint16_t mitad_alta, uint16_t mitad_baja) {
    return (int32_t)(((uint32_t)mitad_alta << 16) | (uint32_t)mitad_baja);
}

static inline uint16_t mitad_alta(int32_t valor) {
    return (uint16_t)(((uint32_t)valor) >> 16);
}

static inline uint16_t mitad_baja(int32_t valor) {
    return (uint16_t)(((uint32_t)valor) & 0xFFFFu);
}

void Ejecutar(MaquinaVirtual *mv);

// Carga en memoria el programa .vmx pasado por parametro, inicializando la TDS y los registros de la máquina virtual.
void cargar_programa(const char *path, MaquinaVirtual *mv);

void inicializar_tds(MaquinaVirtual *mv, int version, uint16_t tam_codigo);
void inicializar_registros(MaquinaVirtual *mv, int version);

// constantes de errores
#define InstInv 1
#define DivCero 2
#define FalloSeg 3

void dirValida(uint32_t puntero, int16_t dirFis, MaquinaVirtual *mv);
int16_t calculaDirFis(int16_t offset, uint32_t puntero, MaquinaVirtual *mv);
void guardaMem(MaquinaVirtual *mv);
void leeMem(MaquinaVirtual *mv);
int32_t valorOperado(int op, MaquinaVirtual *mv);
void infoOperando(MaquinaVirtual *mv);
void decoOperacion(uint8_t operacion, MaquinaVirtual *mv);
void leeOperacion(uint8_t *operacion, MaquinaVirtual *mv);
void error(int ce);
void guardaOP(uint8_t op, uint32_t valor, MaquinaVirtual *mv);
void decideGuardar(uint32_t valor1, uint32_t valor2, MaquinaVirtual *mv);


void modificaCC(int32_t ori, int64_t over, uint64_t carry, MaquinaVirtual *mv);

// Función para los códigos no definidos
void NULA(int32_t *a, int32_t *b, MaquinaVirtual *mv);

// Prototipos de las instrucciones
void SYS(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JMP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JN(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JZ(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JC(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JV(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JNP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JNN(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void JNZ(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void NOT(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void STOP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void MOV(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void ADD(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SUB(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void MUL(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void DIV(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void CMP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void AND(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void OR(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void XOR(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SWAP(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SHL(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SHR(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void SAR(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void LDL(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void LDH(int32_t *a, int32_t *b, MaquinaVirtual *mv);
void RND(int32_t *a, int32_t *b, MaquinaVirtual *mv);


// Funciones del disassembler
const char *obtener_mnemonico(unsigned char opcode);
int desensamblar_instruccion(const unsigned char *instr, unsigned short direccion_fisica, char *linea, size_t tam_linea);

void disassembler(const unsigned char *memoria, unsigned short base_cs, unsigned short tam_codigo);
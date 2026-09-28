# ============================================================
#  Makefile - Maquina Virtual MV1 (vmx)
#  UNMDP - Fundamentos de la Arquitectura de Computadoras 2026
#
#  Funciona en Windows 11 (MinGW-w64 / MSYS2 / Git Bash) y Linux.
#  En Windows usar:  mingw32-make   (o make, si esta instalado)
# ============================================================

CC       = gcc
CFLAGS  ?= -std=c11 -Wall -Wextra -O2
INCLUDE  = -Iinclude

SRC_DIR   = src
BUILD_DIR = build
BIN_DIR   = bin

# Traductor de la catedra y programas de prueba
VMT_DIR   = tools
TESTS_ASM = tests/asm
TESTS_VMX = tests/vmx

# Deteccion de sistema operativo
ifeq ($(OS),Windows_NT)
    EXE     = .exe
    VMT     = $(VMT_DIR)/vmt.exe
    MKDIR   = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
    RMDIR   = if exist "$(subst /,\,$(1))" rmdir /S /Q "$(subst /,\,$(1))"
    RMFILE  = if exist "$(subst /,\,$(1))" del /Q "$(subst /,\,$(1))"
    # Si se corre desde Git Bash / MSYS2, usar comandos POSIX
    ifneq ($(findstring sh,$(SHELL)),)
        MKDIR  = mkdir -p $(1)
        RMDIR  = rm -rf $(1)
        RMFILE = rm -f $(1)
    endif
else
    EXE     =
    VMT     = $(VMT_DIR)/vmt
    MKDIR   = mkdir -p $(1)
    RMDIR   = rm -rf $(1)
    RMFILE  = rm -f $(1)
endif

TARGET  = $(BIN_DIR)/vmx$(EXE)
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS    = $(OBJECTS:.o=.d)

# Argumentos para "make run": make run PROG=tests/vmx/x.vmx ARGS=-d
PROG ?= tests/vmx/hola.vmx
ARGS ?=

.PHONY: all release debug run disasm test clean help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@$(call MKDIR,$(BIN_DIR))
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(call MKDIR,$(BUILD_DIR))
	$(CC) $(CFLAGS) $(INCLUDE) -MMD -MP -c $< -o $@

-include $(DEPS)

# Compilacion optimizada / con simbolos de depuracion
release: CFLAGS = -std=c11 -Wall -Wextra -O2 -DNDEBUG
release: clean all

debug: CFLAGS = -std=c11 -Wall -Wextra -g -O0
debug: clean all

# Traduce un .asm a .vmx con vmt:  make tests/vmx/hola.vmx
$(TESTS_VMX)/%.vmx: $(TESTS_ASM)/%.asm
	@$(call MKDIR,$(TESTS_VMX))
	$(VMT) $< $@

# Ejecuta la VM sobre PROG (por defecto tests/vmx/hola.vmx)
run: $(TARGET) $(PROG)
	$(TARGET) $(PROG) $(ARGS)

# Igual que run, pero mostrando el disassembler
disasm: ARGS = -d
disasm: run

# Traduce todos los .asm de tests/asm y ejecuta cada uno
ASM_FILES = $(wildcard $(TESTS_ASM)/*.asm)
VMX_FILES = $(patsubst $(TESTS_ASM)/%.asm,$(TESTS_VMX)/%.vmx,$(ASM_FILES))

test: $(TARGET) $(VMX_FILES)
	@for f in $(VMX_FILES); do echo "=== $$f ==="; $(TARGET) $$f || exit 1; done

clean:
	@$(call RMDIR,$(BUILD_DIR))
	@$(call RMDIR,$(BIN_DIR))

help:
	@echo "Targets disponibles:"
	@echo "  make            Compila la VM en $(TARGET)"
	@echo "  make release    Recompila optimizado"
	@echo "  make debug      Recompila con simbolos de depuracion (-g)"
	@echo "  make run        Ejecuta PROG (default: $(PROG))"
	@echo "  make disasm     Ejecuta PROG con el disassembler (-d)"
	@echo "  make test       Traduce y ejecuta todos los .asm de $(TESTS_ASM)"
	@echo "  make clean      Elimina $(BUILD_DIR)/ y $(BIN_DIR)/"
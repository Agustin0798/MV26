# Máquina Virtual MV1 — Parte I

**UNMDP – Facultad de Ingeniería | Fundamentos de la Arquitectura de Computadoras — MV1 2026**

Implementación de una máquina virtual (`vmx`) que emula la ejecución de programas escritos en el Assembler de la cátedra, previamente traducidos a lenguaje máquina (`.vmx`) con el traductor `vmt` provisto por la cátedra.

| Proceso | Componente | Entrada | Salida |
|---|---|---|---|
| Traducción | `vmt` (cátedra) | `*.asm` | `*.vmx` |
| Ejecución | `vmx` (**este proyecto**) | `*.vmx` | Resultados en pantalla |

## Lenguaje y entorno

- Lenguaje: **C** (estándar C11).
- Desarrollado en **Windows 11** con **MinGW-w64 (GCC)**. También compila en Linux/macOS.
- Sistema de build: **Make** (`Makefile` incluido).


## Requisitos

- **GCC** (en Windows: [MSYS2](https://www.msys2.org/) o [WinLibs](https://winlibs.com/) con MinGW-w64).
- **GNU Make** (en Windows se llama `mingw32-make`).

Instalación en Windows con MSYS2 (terminal *MSYS2 UCRT64*):

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
```

Agregar `C:\msys64\ucrt64\bin` al `PATH` y verificar:

```bash
gcc --version
mingw32-make --version
```

## Compilación

Desde la raíz del proyecto (en Windows usar `mingw32-make`; en Linux/macOS, `make`):

```bash
mingw32-make            # compila y genera bin/vmx.exe
```

Compilación manual, sin Make:

```bash
gcc -std=c11 -Wall -Wextra -O2 -Iinclude src/*.c -o bin/vmx.exe
```

## Uso

### 1. Traductor (provisto por la cátedra)

```
vmt filename.asm [filename.vmx] [-o]
```

- `filename.asm` (obligatorio): archivo fuente Assembler.
- `filename.vmx` (opcional): nombre del archivo de salida. Si se omite, usa el mismo nombre con extensión `.vmx` (sobrescribe si existe).
- `-o` (opcional): omite la salida por pantalla de la traducción (no omite los errores).

```bash
tools\vmt.exe tests\asm\hola.asm tests\vmx\hola.vmx
```

### 2. Máquina virtual

```
vmx filename.vmx [-d]
```

- `filename.vmx` (obligatorio): binario en lenguaje máquina a ejecutar.
- `-d` (opcional): muestra el código desensamblado (disassembler) del programa cargado en memoria.

```bash
bin\vmx.exe tests\vmx\hola.vmx        # ejecuta el programa
bin\vmx.exe tests\vmx\hola.vmx -d     # muestra el desensamblado y ejecuta
```


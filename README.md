# Máquina Virtual para Arquitectura de Computadoras 2026 — Parte I

**UNMDP – Facultad de Ingeniería | Fundamentos de la Arquitectura de Computadoras**

Implementación de una máquina virtual capaz de interpretar y ejecutar programas escritos en el lenguaje Assembler definido por la cátedra, previamente traducidos a lenguaje máquina (`.vmx`) mediante el traductor `vmt` provisto por la cátedra.



## Lenguaje

Este proyecto está desarrollado en **C++** (C++17), utilizando **CMake** como sistema de build.

La máquina virtual interpreta el lenguaje Assembler propio de la cátedra (MV1), cuya referencia completa —sintaxis, tipos de operando, instrucciones y llamadas al sistema— está detallada en [`docs/lenguaje_assembler.md`](docs/lenguaje_assembler.md).


## Requisitos

- **CMake** ≥ 3.15
- Compilador C++ compatible con C++17 (GCC, Clang o MSVC)
- Sistema de build (Make, Ninja o el generador de tu preferencia)

## Compilación (Esto es para quemar a Agus que no sabe usar CMake)

El proyecto usa un build **fuera del árbol de fuentes** (out-of-source), generando los binarios en una carpeta `build/` independiente.

1. Crear y entrar al directorio de build:
   ```bash
   mkdir build && cd build
   ```

2. Configurar el proyecto con CMake:
   ```bash
   cmake ..
   ```
   *(En Windows con Visual Studio, se puede especificar el generador, por ejemplo: `cmake .. -G "Visual Studio 17 2022"`)*

3. Compilar:
   ```bash
   cmake --build .
   ```
   Esto genera el ejecutable `vmx` (o `vmx.exe` en Windows) dentro de `build/` o `bin/`, según la configuración del `CMakeLists.txt`.

> Para una compilación en modo Release: `cmake .. -DCMAKE_BUILD_TYPE=Release`

## Ejecución

### 1. Traducir un programa Assembler a lenguaje máquina

Usando el traductor de la cátedra (`tools/vmt/vmt.exe`):

```bash
tools/vmt/vmt.exe tests/asm/programa.asm tests/vmx/programa.vmx
```

### 2. Ejecutar el programa en la máquina virtual

```bash
bin/vmx tests/vmx/programa.vmx
```

### 3. Ejecutar en modo disassembler

Muestra el código Assembler equivalente al programa cargado en memoria, además de ejecutarlo:

```bash
bin/vmx tests/vmx/programa.vmx -d
```

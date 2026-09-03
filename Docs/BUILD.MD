
## Requisitos

- CMake >= 3.16
- Compilador C (GCC/MinGW en Windows, GCC/Clang en Linux/Mac)

## Compilar

\```bash
cmake -B build -S .
cmake --build build
\```

## Ejecutar

\```bash
./build/mv26
\```

## Cross-compilation a Windows (desde Linux)

Ver [docs/BUILD.md](docs/BUILD.md) para setup del toolchain MinGW.

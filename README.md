# LRUCache

Implementación genérica de una caché LRU (Least Recently Used) en C++20 con complejidad temporal O(1) amortizada para todas las operaciones principales.

## Características

- Operaciones put, get y remove en O(1) amortizado.
- Reubicación de nodos en O(1) sin realocaciones en el heap mediante std::list::splice.
- Biblioteca header-only genérica mediante templates (Key, Value).
- Sincronización para acceso concurrente mediante std::mutex y std::lock_guard.
- Sin dependencias externas, compatible con compiladores C++20.

## Estructura 

### Aclaracion: Outputs sin tilde y sin "ñ" para evitar problemas de codificacion y visualizacion en determinadas terminales

```
LRUCache.cpp/
├── CMakeLists.txt
├── README.md
├── include/
│   └── LRUCache.hpp
├── src/
│   └── main.cpp
└── tests/
    └── LRUCacheTests.cpp
```

## Compilación y Pruebas

### Con CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

### Directo con g++

Compilar y ejecutar la demostración:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp -o lru_demo
./lru_demo
```

Compilar y ejecutar las pruebas:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Iinclude tests/LRUCacheTests.cpp -o lru_tests
./lru_tests
```

## Salida de Pruebas

```
--- Suite de Pruebas: LRUCache ---

[TEST] Ejecutando test_basic_operations...
       -> OK
[TEST] Ejecutando test_eviction_policy...
       -> OK
[TEST] Ejecutando test_update_existing_key...
       -> OK
[TEST] Ejecutando test_remove_and_clear...
       -> OK
[TEST] Ejecutando test_concurrency (Multi-threading)...
       -> OK (Sin data races ni deadlocks)

[RESULTADO] ¡Todas las pruebas unitarias pasaron exitosamente!
```

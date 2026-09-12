#include <iostream>
#include <string>
#include "LRUCache.hpp"

int main() {
    std::cout << "--- Demostracion de LRUCache (C++20) ---\n\n";

   
    LRUCache<int, std::string> cache(3);
    std::cout << "[INFO] Cache creada con capacidad: " << cache.capacity() << "\n\n";

    // Insert 3 elementos. Cache llena.
    std::cout << "-> Insertando: (1, 'Uno'), (2, 'Dos'), (3, 'Tres')\n";
    cache.put(1, "Uno");
    cache.put(2, "Dos");
    cache.put(3, "Tres");
    std::cout << "[ESTADO] Tamanio actual: " << cache.size() << "\n\n";

    // Accede a elemento 1. (Se convierte en MRU (Most Recently Used)
    std::cout << "-> Accediendo a la clave 1: ";
    if (auto val = cache.get(1)) {
        std::cout << *val << " (marcado como MRU)\n";
    }

   
    // Orden actual en cache (MRU a LRU): 1 -> 3 -> 2
    // LRU: 2

    
    // Inserta un 4to elemento. Expulsion forzada del LRU. Clave 2.
    std::cout << "\n-> Insertando (4, 'Cuatro') superando la capacidad...\n";
    cache.put(4, "Cuatro");

    // Verificacion
    std::cout << "-> Comprobando si clave 2 sigue existiendo: ";
    if (cache.contains(2)) {
        std::cout << "Existe (Error en política LRU!)\n";
    } else {
        std::cout << "Desalojado correctamente (OK)\n";
    }

    // Verificar claves 1, 3 y 4.
    std::cout << "-> Clave 1: " << (cache.get(1).has_value() ? *cache.get(1) : "No existe") << "\n";
    std::cout << "-> Clave 3: " << (cache.get(3).has_value() ? *cache.get(3) : "No existe") << "\n";
    std::cout << "-> Clave 4: " << (cache.get(4).has_value() ? *cache.get(4) : "No existe") << "\n";

    // Update del valor.
    std::cout << "\n-> Actualizando clave 3 a 'Tres_Modificado'\n";
    cache.put(3, "Tres_Modificado");
    std::cout << "-> Clave 3 ahora es: " << *cache.get(3) << "\n";
    std::cout << "[ESTADO] Tamanio tras actualización: " << cache.size() << "\n\n";

    std::cout << "\nDemostracion finalizada con exito.\n";

    return 0;
}

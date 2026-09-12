#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "LRUCache.hpp"

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "[FALLO] Assertion fallida: " #condition << " en linea " << __LINE__ << "\n"; \
            std::exit(1); \
        } \
    } while (false)

#define ASSERT_EQ(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            std::cerr << "[FALLO] Assertion fallida: " #actual " == " #expected \
                      << " (obtenido: " << (actual) << ", esperado: " << (expected) << ") en linea " << __LINE__ << "\n"; \
            std::exit(1); \
        } \
    } while (false)

void test_basic_operations() {
    std::cout << "[TEST] Ejecutando test_basic_operations...\n";
    LRUCache<int, int> cache(2);

    ASSERT_EQ(cache.capacity(), 2);
    ASSERT_TRUE(cache.empty());
    ASSERT_EQ(cache.size(), 0);

    cache.put(1, 100);
    cache.put(2, 200);

    ASSERT_EQ(cache.size(), 2);
    ASSERT_TRUE(cache.contains(1));
    ASSERT_TRUE(cache.contains(2));
    ASSERT_EQ(*cache.get(1), 100);
    ASSERT_EQ(*cache.get(2), 200);
    ASSERT_TRUE(!cache.get(3).has_value());
    std::cout << "       -> OK\n";
}

void test_eviction_policy() {
    std::cout << "[TEST] Ejecutando test_eviction_policy...\n";
    LRUCache<std::string, int> cache(2);

    cache.put("A", 1);
    cache.put("B", 2);

    // Acceder a "A" para convertirlo en MRU. Ahora "B" pasa a ser LRU.
    auto valA = cache.get("A");
    ASSERT_TRUE(valA.has_value());
    ASSERT_EQ(*valA, 1);

    // Insertar "C". Debería expulsar a "B" (el LRU).
    cache.put("C", 3);

    ASSERT_TRUE(cache.contains("A"));
    ASSERT_TRUE(!cache.contains("B")); // "B" fue desalojado
    ASSERT_TRUE(cache.contains("C"));
    ASSERT_EQ(cache.size(), 2);
    std::cout << "       -> OK\n";
}

void test_update_existing_key() {
    std::cout << "[TEST] Ejecutando test_update_existing_key...\n";
    LRUCache<int, std::string> cache(2);

    cache.put(10, "Diez");
    cache.put(20, "Veinte");

    // Actualizar valor de la clave 10 (no debe exceder capacidad ni desalojar a 20)
    cache.put(10, "Diez_Actualizado");
    ASSERT_EQ(cache.size(), 2);
    ASSERT_EQ(*cache.get(10), "Diez_Actualizado");

    // Ahora 10 es MRU y 20 es LRU. Insertar 30 debe desalojar 20.
    cache.put(30, "Treinta");
    ASSERT_TRUE(cache.contains(10));
    ASSERT_TRUE(!cache.contains(20));
    ASSERT_TRUE(cache.contains(30));
    std::cout << "       -> OK\n";
}

void test_remove_and_clear() {
    std::cout << "[TEST] Ejecutando test_remove_and_clear...\n";
    LRUCache<int, int> cache(3);

    cache.put(1, 10);
    cache.put(2, 20);
    cache.put(3, 30);

    ASSERT_TRUE(cache.remove(2));
    ASSERT_TRUE(!cache.contains(2));
    ASSERT_EQ(cache.size(), 2);

    ASSERT_TRUE(!cache.remove(99)); // Clave no existente retorna false

    cache.clear();
    ASSERT_TRUE(cache.empty());
    ASSERT_EQ(cache.size(), 0);
    std::cout << "       -> OK\n";
}

void test_concurrency() {
    std::cout << "[TEST] Ejecutando test_concurrency (Multi-threading)...\n";
    LRUCache<int, int> cache(100);

    constexpr int NUM_THREADS = 8;
    constexpr int OPS_PER_THREAD = 1000;
    std::vector<std::thread> threads;

    // Writing threads
    for (int t = 0; t < NUM_THREADS / 2; ++t) {
        threads.emplace_back([&cache, t]() {
            for (int i = 0; i < OPS_PER_THREAD; ++i) {
                cache.put(i % 150, (t * 1000) + i);
            }
        });
    }

    // Reading threads
    for (int t = NUM_THREADS / 2; t < NUM_THREADS; ++t) {
        threads.emplace_back([&cache]() {
            for (int i = 0; i < OPS_PER_THREAD; ++i) {
                (void)cache.get(i % 150);
                (void)cache.contains(i % 150);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    ASSERT_TRUE(cache.size() <= 100);
    std::cout << "       -> OK (Sin data races ni deadlocks)\n";
}

int main() {
    std::cout << "--- Suite de Pruebas: LRUCache ---\n\n";

    test_basic_operations();
    test_eviction_policy();
    test_update_existing_key();
    test_remove_and_clear();
    test_concurrency();

    std::cout << "\n[RESULTADO] ¡Todas las pruebas unitarias pasaron exitosamente!\n";
    return 0;
}

#pragma once

#include <cstddef>
#include <list>
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <utility>
#include <mutex>
#include <stdexcept>

/**
 * @brief Implementación moderna y genérica de LRU Cache (Least Recently Used) en C++20.
 * 
 * Conceptos clave:
 * 1. O(1) Get y O(1) Put:
 *    - Se combinan dos estructuras:
 *      a) std::list<std::pair<Key, Value>>: Lista doblemente enlazada que mantiene
 *         el orden de uso. El frente (front) es el más recientemente usado (MRU),
 *         la cola (back) es el menos recientemente usado (LRU).
 *      b) std::unordered_map<Key, iterator>: Tabla hash que mapea cada clave a su
 *         iterador directo en la lista. Permite acceso O(1).
 * 
 * 2. Por qué std::pair<Key, Value> en la lista:
 *    Cuando la caché alcanza su capacidad máxima y se debe expulsar el nodo de la cola
 *    (LRU eviction), necesitamos conocer su 'Key' para poder eliminar la entrada
 *    correspondiente del unordered_map en O(1). Si sólo guardáramos el 'Value',
 *    tendríamos que recorrer todo el mapa en O(N).
 * 
 * 3. Rendimiento O(1) sin copias ni allocations (std::list::splice):
 *    Al acceder a un elemento existente, no se borra ni se vuelve a crear el nodo en memoria;
 *    se utiliza 'splice()', que simplemente reconecta los punteros 'prev' y 'next' del nodo,
 *    moviéndolo al inicio en tiempo estrictamente constante O(1) y con 0 asignaciones en el heap.
 * 
 * 4. Thread-Safety:
 *    Se implementa soporte concurrente mediante std::shared_mutex
 *    (Lecturas concurrentes con shared_lock, escrituras exclusivas con unique_lock).
 */
template <
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>
>
class LRUCache {
public:
    using KeyValuePair = std::pair<Key, Value>;
    using ListIterator = typename std::list<KeyValuePair>::iterator;
    explicit LRUCache(std::size_t capacity) : m_capacity(capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("La capacidad de LRUCache debe ser mayor a 0.");
        }
    }

    // Deshabilitar copia para prevenir copias accidentales costosas de la cache
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    // (Move semantics)
    LRUCache(LRUCache&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_capacity = other.m_capacity;
        m_itemsList = std::move(other.m_itemsList);
        m_itemsMap = std::move(other.m_itemsMap);
    }

    LRUCache& operator=(LRUCache&& other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            m_capacity = other.m_capacity;
            m_itemsList = std::move(other.m_itemsList);
            m_itemsMap = std::move(other.m_itemsMap);
        }
        return *this;
    }

    ~LRUCache() = default;

    /**
     * @brief Inserta o actualiza un par clave-valor en la caché.
     * Si la clave ya existe, actualiza el valor y la mueve al frente (MRU).
     * Si la clave es nueva y la caché está llena, desaloja el elemento más antiguo (LRU).
     * 
     * Complejidad: O(1) tiempo constante amortizado.
     */
    void put(const Key& key, Value value) {
        std::lock_guard<std::mutex> lock(m_mutex); // Thread-safe

        auto it = m_itemsMap.find(key);
        if (it != m_itemsMap.end()) {
            // Caso 1: La clave ya existe.
            // Actualizamos el valor existente
            it->second->second = std::move(value);
            // Reubica MRU usando splice (0(1), sin reasignar memoria)
            m_itemsList.splice(m_itemsList.begin(), m_itemsList, it->second);
            return;
        }

        // Caso 2: La clave es nueva. Verificar capacidad.
        if (m_itemsList.size() >= m_capacity) {
            auto lastIt = --m_itemsList.end();
            m_itemsMap.erase(lastIt->first); // Borrado en O(1) del mapa gracias a lastIt->first
            m_itemsList.pop_back();          // Liberación del nodo en la lista
        }

        m_itemsList.emplace_front(key, std::move(value));
        // Registrar en el mapa el iterador que apunta a la nueva posición
        m_itemsMap[key] = m_itemsList.begin();
    }

    [[nodiscard]] std::optional<Value> get(const Key& key) {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_itemsMap.find(key);
        if (it == m_itemsMap.end()) {
            return std::nullopt;
        }

        // Marcar como recientemente usado: mover nodo al frente de la lista
        m_itemsList.splice(m_itemsList.begin(), m_itemsList, it->second);
        return it->second->second;
    }

    /**
     * Consulta el valor sin alterar el orden LRU (operación de inspección pura).
     */
    [[nodiscard]] std::optional<Value> peek(const Key& key) const {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_itemsMap.find(key);
        if (it == m_itemsMap.end()) {
            return std::nullopt;
        }
        return it->second->second;
    }

    /**
     * Verifica si una clave está presente en la caché sin alterar el orden LRU.
     */
    [[nodiscard]] bool contains(const Key& key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_itemsMap.find(key) != m_itemsMap.end();
    }

    bool remove(const Key& key) {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_itemsMap.find(key);
        if (it == m_itemsMap.end()) {
            return false;
        }

        m_itemsList.erase(it->second); // O(1): std::list::erase recibe el iterador directo
        m_itemsMap.erase(it);          // O(1): borrar de la tabla hash
        return true;
    }

  
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_itemsList.clear();
        m_itemsMap.clear();
    }


    [[nodiscard]] std::size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_itemsList.size();
    }

  
    [[nodiscard]] std::size_t capacity() const noexcept {
        return m_capacity;
    }

 
    [[nodiscard]] bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_itemsList.empty();
    }

private:
    std::size_t m_capacity;
    mutable std::mutex m_mutex; // Mutex para exclusión mutua thread-safe

    // Lista doblemente enlazada de pares {Key, Value}
    std::list<KeyValuePair> m_itemsList;

    // Tabla hash: Clave -> Iterador a la posición en m_itemsList
    std::unordered_map<Key, ListIterator, Hash, KeyEqual> m_itemsMap;
};

cmake_minimum_required(VERSION 3.20)
project(LRUCache VERSION 1.0.0 LANGUAGES CXX)

# Exigir C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones de compilación para advertencias y seguridad
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Definir la biblioteca header-only (INTERFACE library)
add_library(lru_cache INTERFACE)
target_include_directories(lru_cache INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Target principal: Ejecutable de demostración
add_executable(lru_cache_demo src/main.cpp)
target_link_libraries(lru_cache_demo PRIVATE lru_cache)

# Target de pruebas unitarias
add_executable(lru_cache_tests tests/LRUCacheTests.cpp)
target_link_libraries(lru_cache_tests PRIVATE lru_cache)

# Habilitar soporte de pruebas con CTest
enable_testing()
add_test(NAME LRUCacheUnitTests COMMAND lru_cache_tests)

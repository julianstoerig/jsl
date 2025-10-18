cmake_minimum_required(VERSION 3.31)

project(JSL
        VERSION 0.3
        LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CTEST_CMAKE_GENERATOR Ninja)
set(CTEST_CONFIGURATION_TYPE Debug)
set(CTEST_MEMORYCHECK_TYPE AddressSanitizer)

enable_testing()

add_executable(test_trivial base.c test_trivial.c)
add_test(NAME test_trivial COMMAND test_trivial)

add_executable(test_stretchy_buffer_basic base.c test_stretchy_buffer_basic.c)
add_test(NAME test_stretchy_buffer_basic COMMAND test_stretchy_buffer_basic)

add_executable(test_hash_unhash base.c test_hash_unhash.c)
add_test(NAME test_hash_unhash COMMAND test_hash_unhash)

add_executable(test_stretchy_buffer_fib base.c test_stretchy_buffer_fib.c)
add_test(NAME test_stretchy_buffer_fib COMMAND test_stretchy_buffer_fib)

add_executable(test_arena base.c test_arena.c)
add_test(NAME test_arena COMMAND test_arena)

add_executable(test_arena_auto_init base.c test_arena_auto_init.c)
add_test(NAME test_arena_auto_init COMMAND test_arena_auto_init)

add_executable(test_da base.c test_da.c)
add_test(NAME test_da COMMAND test_da)

add_executable(test_da_arena base.c test_da_arena.c)
add_test(NAME test_da_arena COMMAND test_da_arena)

add_executable(test_ppm base.c test_ppm.c)
add_test(NAME test_ppm COMMAND test_ppm)

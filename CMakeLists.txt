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

add_executable(test_trivial test_trivial.c)
add_test(NAME test_trivial COMMAND test_trivial)

add_executable(test_machine_eps test_machine_eps.c)
add_test(NAME test_machine_eps COMMAND test_machine_eps)

add_executable(test_stretchy_buffer_basic test_stretchy_buffer_basic.c)
add_test(NAME test_stretchy_buffer_basic COMMAND test_stretchy_buffer_basic)

add_executable(test_hash_unhash test_hash_unhash.c)
add_test(NAME test_hash_unhash COMMAND test_hash_unhash)

add_executable(test_stretchy_buffer_fib test_stretchy_buffer_fib.c)
add_test(NAME test_stretchy_buffer_fib COMMAND test_stretchy_buffer_fib)

add_executable(test_arena test_arena.c)
add_test(NAME test_arena COMMAND test_arena)

add_executable(test_arena_auto_init test_arena_auto_init.c)
add_test(NAME test_arena_auto_init COMMAND test_arena_auto_init)

add_executable(test_da test_da.c)
add_test(NAME test_da COMMAND test_da)

add_executable(test_ppm test_ppm.c)
add_test(NAME test_ppm COMMAND test_ppm)

add_executable(test_stack test_stack.c)
add_test(NAME test_stack COMMAND test_stack)

add_executable(test_queue test_queue.c)
add_test(NAME test_queue COMMAND test_queue)

add_executable(test_dll test_dll.c)
add_test(NAME test_dll COMMAND test_dll)

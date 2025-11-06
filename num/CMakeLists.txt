cmake_minimum_required(VERSION 4.00) # lowest tested, feel free to bump down

project(
    num
    VERSION 0.0.1
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CTEST_CMAKE_GENERATOR Ninja)
set(CTEST_CONFIGURATION_TYPE Debug)
set(CTEST_MEMORYCHECK_TYPE AddressSanitizer)

enable_testing()

add_executable(test_isclose num.c test_isclose.c)
add_test(NAME test_isclose COMMAND test_isclose)
target_link_libraries(test_isclose m)

add_executable(test_bisection num.c test_bisection.c)
add_test(NAME test_bisection COMMAND test_bisection)
target_link_libraries(test_bisection m)

add_executable(test_secant num.c test_secant.c)
add_test(NAME test_secant COMMAND test_secant)
target_link_libraries(test_secant m)

add_executable(test_newton num.c test_newton.c)
add_test(NAME test_newton COMMAND test_newton)
target_link_libraries(test_newton m)

add_executable(test_fixpoint num.c test_fixpoint.c)
add_test(NAME test_fixpoint COMMAND test_fixpoint)
target_link_libraries(test_fixpoint m)

add_executable(test_secant_bisect num.c test_secant_bisect.c)
add_test(NAME test_secant_bisect COMMAND test_secant_bisect)
target_link_libraries(test_secant_bisect m)

add_executable(test_write_values num.c test_write_values.c)
add_test(NAME test_write_values COMMAND test_write_values)
target_link_libraries(test_write_values m)

add_executable(test_matrix num.c test_matrix.c)
add_test(NAME test_matrix COMMAND test_matrix)
target_link_libraries(test_matrix m)

add_executable(test_integration num.c test_integration.c)
add_test(NAME test_integration COMMAND test_integration)
target_link_libraries(test_integration m)

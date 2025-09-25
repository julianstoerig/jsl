cmake_minimum_required(VERSION 3.20)

project(dsh
    VERSION 0.0.1
    LANGUAGES C
)

enable_testing()

add_executable(test_trivial test_trivial.c)
#add_executable(test_arena test_arena.c)

add_test(NAME Test-Trivial COMMAND test_trivial)
#add_test(NAME Test-Arena COMMAND test_arena)

cmake_minimum_required(VERSION 3.20)

project(dsh
    VERSION 0.0.1
    LANGUAGES C
)

enable_testing()

add_executable(test_trivial
    test_trivial.c
    ds.c
)

add_test(NAME Test-Trivial COMMAND test_trivial)

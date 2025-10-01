cmake_minimum_required(VERSION 3.31)

project(JSL
        VERSION 0.1
        LANGUAGES C)

enable_testing()

add_executable(test_trivial base.c test_trivial.c)
add_test(NAME test_trivial COMMAND test_trivial)

add_executable(test_hash_unhash base.c test_hash_unhash.c)
add_test(NAME test_hash_unhash COMMAND test_hash_unhash)

cmake_minimum_required(VERSION 4.1)

project(JSL
        VERSION 0.1
        LANGUAGES CXX C)

enable_testing()

add_executable(test_trivial base.cpp test_trivial.cpp)
add_test(NAME test_trivial COMMAND test_trivial)

add_executable(test_hash_unhash base.cpp test_hash_unhash.cpp)
add_test(NAME test_hash_unhash COMMAND test_hash_unhash)

cmake_minimum_required(VERSION 3.31)

project(JSL
        VERSION 0.2
        LANGUAGES C)

enable_testing()

add_executable(test_trivial base.c test_trivial.c)
add_test(NAME test_trivial COMMAND test_trivial)

add_executable(test_sb_push_len_index_free base.c test_sb_push_len_index_free.c)
add_test(NAME test_sb_push_len_index_free COMMAND test_sb_push_len_index_free)

add_executable(test_hash_unhash base.c test_hash_unhash.c)
add_test(NAME test_hash_unhash COMMAND test_hash_unhash)

add_executable(test_sb_fibonacci base.c test_sb_fibonacci.c)
add_test(NAME test_sb_fibonacci COMMAND test_sb_fibonacci)

add_executable(test_arena base.c test_arena.c)
add_test(NAME test_arena COMMAND test_arena)

add_executable(test_da base.c test_da.c)
add_test(NAME test_da COMMAND test_da)

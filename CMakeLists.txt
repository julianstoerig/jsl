cmake_minimum_required(VERSION 3.20)
project(jsl)
add_executable(
  jsl
  main.c
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_compile_options(
  jsl
  PRIVATE
  -std=c2x
  -Wall
  -Wextra
  -pedantic
  -O1
  -g3
  -ggdb
)

target_link_libraries(
  jsl
  -lc
  -lm
)

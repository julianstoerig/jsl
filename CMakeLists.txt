cmake_minimum_required(VERSION 3.20)
project(jsl)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_executable(
  jsl
  main.c
)

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

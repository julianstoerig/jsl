/*
This file is modified from the source code found at the following website,
distrbuted under the following license ("MIT License"):
https://github.com/AxlLind/libwebb/tree/main at 2025-09-16T23:19:41

MIT License

Copyright (c) 2023 Axel Lindeberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Example of Usage:

TEST(test_trivial) {
    ASSERT(2+2==4);
}

TEST_MAIN(
    test_trivial,
)

*/

#ifndef LIBTEST_H
#define LIBTEST_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define LT_FILENAME (strrchr("/" __FILE__, '/') + 1)

#define LT_INTERNAL_ASSERT(expr, assert_expr)                         \
  do {                                                             \
    if (!(expr)) {                                                 \
      printf("%s:%d - expected: %s\n", LT_FILENAME, __LINE__, #expr); \
      *test_failed = 1;                                            \
      if (assert_expr)                                             \
        return;                                                    \
    }                                                              \
  } while (0)

#define LT_EXPECT(expr) LT_INTERNAL_ASSERT(expr, 0)
#define LT_ASSERT(expr) LT_INTERNAL_ASSERT(expr, 1)

typedef struct {
  void (*fn)(int *);
  char *name;
} lt_TestInfo;

#define LT_TEST(name)                                 \
  static void name##_fn(int *);                    \
  static const lt_TestInfo name = {name##_fn, #name}; \
  static void name##_fn(int *test_failed)

#define LT_TEST_MAIN(...)                                                      \
  int main(void) {                                                          \
    const lt_TestInfo tests[] = {__VA_ARGS__};                                 \
    int failed_tests = 0;                                                   \
    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {         \
      int test_failed = 0;                                                  \
      tests[i].fn(&test_failed);                                            \
      failed_tests += test_failed;                                          \
      printf("[%s] %s\n", test_failed ? "FAIL" : "PASS", tests[i].name);    \
    }                                                                       \
    return failed_tests ? 1 : 0;                                            \
  }

#endif

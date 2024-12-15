#ifndef JSL_ASSERT_H
#define JSL_ASSERT_H

#include <stdio.h>
#include "meta_macros.h"

#define STREAM_LOG(stream, fmt, ...)    fprintf(stream, "%s@%d: " fmt "\n",\
                            __FILE__, __LINE__,__VA_ARGS__)
#define LOG(fmt, ...) STREAM_LOG(stderr, fmt, __VA_ARGS__)
#define LOG_STDOUT(fmt, ...) STREAM_LOG(stdout, fmt, __VA_ARGS__)

#ifndef ASSERT_BREAK
#define ASSERT_BREAK(c) STMNT( LOG("Assertion failed: %s", TO_STRING(c)); exit(1); )
#endif

#define ASSERT(c) STMNT( if (!(c)) { ASSERT_BREAK(c); } )

#define ERR_EXIT(...) STMNT( fprintf(stderr, __VA_ARGS__); exit(1); )
#define ERR_RETURN(ERRV, ...) STMNT( fprintf(stderr, __VA_ARGS__); return ERRV; } )

#define REQUIRE(c) ASSERT(c)
#define ENSURE(c) ASSERT(c)
#endif

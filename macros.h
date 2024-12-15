#ifndef JSL_MACROS
#define JSL_MACROS

#include <time.h>
#include <stdio.h>
#include <malloc.h>

#include "meta_macros.h"
#include "mathy_macros.h"
#include "assert.h"
#include "bit_fiddling.h"
#include "typedefs.h"

#define global static
#define internal static
#define persistent static

#define IS_STACK_ARRAY(a) ((void *)&a == (void *)a)
#define STACK_ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#define SET_UP_TO(d, n, v)  STMNT( size_t i_, n_; \
                      for ( n_ = (n), i_ = 0; n_ > 0; --n_, ++i_) \
                      (d)[i_] = (v); )
#define ZERO_UP_TO(d, n)    SET_UP_TO(d, n, 0)
#define COLUMNS(S,E)  ( (E) - (S) + 1 )
#define RANGE(i,y,x)  for(i=(y);(((x)>=(y))?(i<(x)):(i>x));\
                      (((x)>=(y))?((i)++):((i)--)))

#define FOREACH(i, A) for(int _keep=1, \
                      _count=0,\
                      _size=sizeof (A) / sizeof *(A); \
                      _keep && _count != _size; \
                      _keep = !_keep, _count++) \
                      for(i = (A)+_count; _keep; _keep = !_keep)

#define LOOP for(;;)
#define MAIN int main(int argc, char** argv)

#define NOW time(NULL)
#define EFFECTIVE_USER_ID geteuid()
#define USER_ID getuid()

#endif

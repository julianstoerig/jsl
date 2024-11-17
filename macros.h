#ifndef JSL_MACROS
#define JSL_MACROS

#include <time.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define ODD(n) ((n) & 1)
#define EVEN(n) (n % 2 == 0)
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define NEG(x) (-(x))
#define SQ(x) ((x)*(x))
#define PI 3.141592653589793238462643
#define E  2.718281828459045235360287
#define RAD2DEG(x) ((x)*180/PI)
#define DEG2RAD(x) ((x)*PI/180)
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CLAMP(x, lo, hi) (MAX((lo), MIN((hi), (x))))
#define DELTA(x, y) (ABS((x)-(y)))
#define SWAP(a, b, type) do { type c = a; type d = b; b = c; a = d; } while ( 0 )
#define COMPARE(a, b) (((a) > (b)) - ((a) < (b)))
#define SIGN(x) (COMPARE((x), 0))
#define IN_RANGE_INCL(x, lo, hi) ((x) >= (lo) && (x) <= (hi))
#define IN_RANGE_EXCL(x, lo, hi) ((x) > (lo) && (x) < (hi))
#define IN_RANGE_UPPER_INCL(x, lo, hi) ((x) > (lo) && (x) <= (hi))
#define IN_RANGE_LOWER_INCL(x, lo, hi) ((x) >= (lo) && (x) < (hi))
#define IMPLIES(a, b) (!(a) || (b))
#define IFF(a, b) ((a) == (b))

#define BIT(x) (1<<(x))
#define SETBIT(x,p) ((x)|(1<<(p)))
#define CLEARBIT(x,p) ((x)&(~(1<<(p))))
#define GETBIT(x,p) (((x)>>(p))&1)
#define TOGGLEBIT(x,p) ((x)^(1<<(p)))

#define IS_STACK_ARRAY(a) ((void *)&a == (void *)a)
#define STACK_ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#define SET_UP_TO(d, n, v)  do{ size_t i_, n_; \
                      for ( n_ = (n), i_ = 0; n_ > 0; --n_, ++i_) \
                      (d)[i_] = (v); } while(0)
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

#define DIE exit(0)

#define ASSERT(cond) if(!(cond)){\
    printf(__FILE__ "@%d: `" #cond "` - Failed | Compilation: " __DATE__ " " __TIME__ "\n", __LINE__);\
    return(-1);}

#define REQUIRE(cond) ASSERT(cond)
#define ENSURE(cond) ASSERT(cond)
#define LOG(x, fmt, ...)    if(x){printf("%s@%d: " fmt "\n",\
                            __FILE__, __LINE__,__VA_ARGS__);}
#define TO_STRING(str)  #str

#endif

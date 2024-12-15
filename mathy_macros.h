#ifndef JSL_MATHY_MACROS_H
#define JSL_MATHY_MACROS_H

#include "meta_macros.h"
#include "typedefs.h"

typedef enum {
    ORD_LESS = -1,
    ORD_EQUAL = 0,
    ORD_GREATER = 1,
} ORDERING;

#define ODD(n) ((n) & 1)
#define EVEN(n) (n % 2 == 0)
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define NEG(x) (-(x))
#define SQR(x) ((x)*(x))
#define PI 3.141592653589793238462643
#define E  2.718281828459045235360287
#define RAD2DEG(x) ((x)*180/PI)
#define DEG2RAD(x) ((x)*PI/180)
#define DELTA(x, y) (ABS((x)-(y)))
#define SWAP(a, b, type) STMNT( type c = a; type d = b; b = c; a = d; )
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CLAMP(x, lo, hi) (MAX((lo), MIN((hi), (x))))
#define CLAMP_BOT(x, lo) MAX(x, lo)
#define CLMAP_TOP(x, hi) MIN(x, hi)

#define COMPARE(a, b) ((ORDERING)(((a) > (b)) - ((a) < (b))))
#define SIGN(x) (((x) > 0) - ((x) < 0))
#define IN_RANGE_INCL(x, lo, hi) ((x) >= (lo) && (x) <= (hi))
#define IN_RANGE_EXCL(x, lo, hi) ((x) > (lo) && (x) < (hi))
#define IN_RANGE_UPPER_INCL(x, lo, hi) ((x) > (lo) && (x) <= (hi))
#define IN_RANGE_LOWER_INCL(x, lo, hi) ((x) >= (lo) && (x) < (hi))
#define IMPLIES(a, b) (!(a) || (b))
#define IFF(a, b) ((a) == (b))

#endif

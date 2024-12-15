
#ifndef JSL_BIT_FIDDLING_H
#define JSL_BIT_FIDDLING_H

#define BYTE(a,b,c,d,e,f,g,h) ((a) << 7 + (b) << 6 + (c) << 5 + (d) << 4 + (e) << 3 + (f) << 2 + (g) << 1 + (h) << 0)

#define NYBBLE(e,f,g,h) ((e) << 3 + (f) << 2 + (g) << 1 + (h) << 0)

#define BIT(x) (1<<(x))
#define SETBIT(x,p) ((x)|(1<<(p)))
#define CLEARBIT(x,p) ((x)&(~(1<<(p))))
#define GETBIT(x,p) (((x)>>(p))&1)
#define TOGGLEBIT(x,p) ((x)^(1<<(p)))

#endif

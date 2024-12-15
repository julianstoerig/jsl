#ifndef JSL_META_MACROS_H
#define JSL_META_MACROS_H

#define INTERNAL_TO_STRING2(s) #s
#define TO_STRING(s) INTERNAL_TO_STRING2(s)

#define INTERNAL_GLUE2(a, b) a##b
#define GLUE(a, b) INTERNAL_GLUE2(a, b)
#define STMNT(macro) do { macro } while (0)


#endif

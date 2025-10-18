#include "base.h"


int main(void) {
    StringBuilder sb = {0};
    char *txt = "Hello World!";
    sb_extend_cstr(&sb, txt);

    if (memcmp(sb.buf, txt, strlen(txt))) return(1);
}

#include <stdio.h>
#include <stdlib.h>


char *js_read_entire_file_and_null_terminate(char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(len);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    fread(buf, 1, len, f);
    fclose(f);
    buf[len-1] = '\0';
    return buf;
}

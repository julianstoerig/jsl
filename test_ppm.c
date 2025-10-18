#include "base.h"
#define STRETCHY_BUFFER_IMPL
#include "stretchy_buffer.h"


typedef enum PPMFormat PPMFormat;
enum PPMFormat {
    PPM_FORMAT_PBM_ASCII = 1,
    PPM_FORMAT_PBM_RAW   = 4,
    PPM_FORMAT_PGM_ASCII = 2,
    PPM_FORMAT_PGM_RAW   = 5,
    PPM_FORMAT_PPM_ASCII = 3,
    PPM_FORMAT_PPM_RAW   = 6,
};

typedef struct PPMImage PPMImage;
struct PPMImage {
    U08 *buf;
    S32 rows;
    S32 cols;
    S32 max_val;
    U08 format;
};

PPMImage ppm_create(U08 *buf, S64 rows, S64 cols, S64 max_val, PPMFormat format) {
    assert(rows >= 0);
    assert(rows <= s32_max_val);
    assert(cols >= 0);
    assert(cols <= s32_max_val);
    assert(max_val >= 0);
    assert(max_val <= (1<<16));

    PPMImage img = {
        .buf = buf,
        .rows = rows,
        .cols = rows,
        .max_val = max_val,
        .format = format,
    };
    return(img);
}

void ppm_write(PPMImage img, Str fname) {
    S32 n_channels = 0;
    B08 is_ascii = 0;
    switch (img.format) {
        case PPM_FORMAT_PBM_ASCII:
            n_channels = 1;
            is_ascii   = 1;
        case PPM_FORMAT_PBM_RAW:
            n_channels = 1;
            is_ascii   = 0;
        case PPM_FORMAT_PGM_ASCII:
            n_channels = 1;
            is_ascii   = 1;
        case PPM_FORMAT_PGM_RAW:
            n_channels = 1;
            is_ascii   = 0;
        case PPM_FORMAT_PPM_ASCII:
            n_channels = 3;
            is_ascii   = 1;
        case PPM_FORMAT_PPM_RAW:
            n_channels = 3;
            is_ascii   = 0;
    }
    S64 bufsize_bytes = img.cols * img.rows * n_channels;
    S64 arena_size = 128 + sizeof(*fname.buf)*fname.len;
    if (is_ascii) arena_size += 2 * bufsize_bytes;

    Arena a = arena_create(arena_size);
    FILE *f = fopen(str_to_cstr(fname, &a), "wb");
    if (!f) goto cleanup;

    char *header_fmt =  "P%d\n"
                        "%d %d\n"
                        "%d\n";
#define PPM_WRITE_HEADER_BUFSIZE 512
    char header[PPM_WRITE_HEADER_BUFSIZE] = {0};
    S64 n = snprintf(header, PPM_WRITE_HEADER_BUFSIZE, header_fmt, img.format, img.rows, img.cols, img.max_val);
    assert(n <= strlen(header_fmt)+strlen("65535")+strlen("10000")*2);

    n = fwrite(header, sizeof(*header), PPM_WRITE_HEADER_BUFSIZE, f);
    if (n+5 < strlen(header)) goto cleanup;

    if (is_ascii) {
        U08 *buf_ascii = make(&a, U08, bufsize_bytes);
        for (S64 i=0; i<bufsize_bytes; ++i) {
            buf_ascii[i];
        }
    } else {
        fwrite(img.buf, sizeof(*img.buf), bufsize_bytes, f);
    }

cleanup:
    if (f) fclose(f);
}

int main(void) {
    Str fname = str_dyn("test_ppm.ppm");

    U08 *xs = {0};

    S64 rows = 720;
    S64 cols = 480;
    S64 channels = 3;
    S64 max_val = 255;

    for (S64 i=0; i<rows*cols*channels; ++i) {
        sb_push(xs, i % max_val);
    }

    PPMImage img = ppm_create(xs, rows, cols, max_val, PPM_FORMAT_PPM_RAW);
    ppm_write(img, fname);
}

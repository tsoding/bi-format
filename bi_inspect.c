// bi.h usage example:
// $ cc -o bi_inspect bi_inspect.c
// $ ./bi_inspect ./samples/*.bi
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define BI_IMPLEMENTATION
#include "bi.h"

#define shift(xs, xs_sz) (assert(xs_sz > 0), xs_sz--, *xs++)

bool read_entire_file(const char *path, char **buffer, size_t *buffer_size)
{
    bool result = true;

    FILE *f = fopen(path, "rb");
    if (f == NULL)                 goto fail;
    if (fseek(f, 0, SEEK_END) < 0) goto fail;
    long m = ftell(f);
    if (m < 0)                     goto fail;
    if (fseek(f, 0, SEEK_SET) < 0) goto fail;

    *buffer_size = m;
    *buffer = malloc(*buffer_size);

    fread(*buffer, *buffer_size, 1, f);
    if (ferror(f)) {
        // TODO: Afaik, ferror does not set errno. So the error reporting in fail is not correct in this case.
        goto fail;
    }

    fclose(f);
    return true;
fail:
    fprintf(stderr, "Could not read file %s: %s", path, strerror(errno));
    if (f) fclose(f);
    return false;
}

int main(int argc, char **argv)
{
    const char *program_name = shift(argv, argc);

    if (argc <= 0) {
        fprintf(stderr, "Usage: %s <input...>\n", program_name);
        fprintf(stderr, "ERROR: no input is provided\n");
        return 1;
    }

    while (argc > 0) {
        char *buffer = 0;
        size_t buffer_size = 0;
        const char *file_path = shift(argv, argc);

        if (read_entire_file(file_path, &buffer, &buffer_size)) {
            Bi bi = {.start = buffer, .count = buffer_size};

            while (!bi_empty(bi)) {
                if (!bi_get_field(&bi)) {
                    fprintf(stderr, "%s[%zu]: Failed to parse a field. Dropping entire file...\n", file_path, bi.field.offset);
                    break;
                }
                switch (bi.field.type) {
                case BI_BLOB:
                    fprintf(stderr, "%s[%zu]: Blob with size %zu bytes\n", file_path, bi.field.offset, bi.field.blob_count);
                    break;
                case BI_INT:
                    fprintf(stderr, "%s[%zu]: Integer with value %zu\n", file_path, bi.field.offset, bi.field.integer);
                    break;
                default:
                    fprintf(stderr, "%s[%zu]: Unexpected field type at offset %zu", file_path, bi.field.offset);
                }
            }
        }

        free(buffer);
    }

    return 0;
}

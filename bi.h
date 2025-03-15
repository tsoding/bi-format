#ifndef BI_H_
#define BI_H_

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    BI_BLOB = 'b',
    BI_INT  = 'i',
} Bi_Type;

typedef struct {
    const char *start;
    size_t count;
    size_t offset;

    struct {
        size_t offset;          // offset at which the field starts
        char type;              // BI_BLOB or BI_INT, might be something else if the file is incorrect, always check for that
        const char *name;
        size_t name_count;

        size_t integer;         // acts like size of the blob when type is BI_BLOB
        const char *blob_start;
    } field;
} Bi;

#define bi_is_empty(bi)  ((bi).offset >= (bi).count)
#define bi_is_digit(x)   ('0' <= (x) && (x) <= '9') // so we don't depend on ctype.h
#define bi_chop_byte(bi) ((bi)->start[(bi)->offset++])

// Create a new Bi parser out of the blob field fetched with bi_get_field().
// Useful to parse .bi recursively.
Bi bi_of_blob(Bi bi);

// Parse the next field into bi->field. Returns true on success, returns false otherwise
bool bi_get_field(Bi *bi);

#endif // BI_H_

#ifdef BI_IMPLEMENTATION

Bi bi_of_blob(Bi bi)
{
    assert(bi.field.type == BI_BLOB);
    return (Bi) {
        .start = bi.field.blob_start,
        .count = bi.field.integer,
    };
}

bool bi_get_field(Bi *bi)
{
    bi->field.offset = bi->offset;

    if (bi_is_empty(*bi) || bi_chop_byte(bi) != ':') return false;

    if (bi_is_empty(*bi)) return false;
    bi->field.type = bi_chop_byte(bi);

    if (bi_is_empty(*bi) || bi_chop_byte(bi) != ' ') return false;

    bi->field.name = &bi->start[bi->offset];
    bi->field.name_count = 0;
    while (!bi_is_empty(*bi) && bi->start[bi->offset] != ' ') {
        bi->field.name_count++;
        bi->offset++;
    }

    if (bi_is_empty(*bi) || bi_chop_byte(bi) != ' ') return false;

    bi->field.integer = 0;
    while (!bi_is_empty(*bi) && bi_is_digit(bi->start[bi->offset])) {
        bi->field.integer *= 10;
        bi->field.integer += bi_chop_byte(bi);
        bi->field.integer -= '0';
    }

    if (bi_is_empty(*bi) || bi_chop_byte(bi) != '\n') return false;

    bi->field.blob_start = NULL;
    if (bi->field.type == BI_BLOB) {
        bi->field.blob_start = &bi->start[bi->offset];
        bi->offset += bi->field.integer;
        if (bi_is_empty(*bi) || bi_chop_byte(bi) != '\n') return false;
        return true;
    }

    return true;
}

#endif // BI_IMPLEMENTATION

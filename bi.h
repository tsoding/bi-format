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
        char type;
        size_t offset;
        const char *name;
        size_t name_count;

        const char *blob_start;
        union {
            size_t integer;
            size_t blob_count;
        };
    } field;
} Bi;

#define bi_empty(bi) ((bi).offset >= (bi).count)

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
        .count = bi.field.blob_count,
    };
}

bool bi_get_field(Bi *bi)
{
    bi->field.offset = bi->offset;

    if (bi->offset >= bi->count || bi->start[bi->offset++] != ':') return false;

    if (bi->offset >= bi->count) return false;
    bi->field.type = bi->start[bi->offset++];

    if (bi->offset >= bi->count || bi->start[bi->offset++] != ' ') return false;

    bi->field.name = &bi->start[bi->offset];
    while (bi->offset < bi->count && bi->start[bi->offset] != ' ') {
        bi->field.name_count++;
        bi->offset++;
    }

    if (bi->offset >= bi->count || bi->start[bi->offset++] != ' ') return false;

    bi->field.integer = 0;
    while (bi->offset < bi->count && '0' <= bi->start[bi->offset] && bi->start[bi->offset] <= '9') {
        bi->field.integer *= 10;
        bi->field.integer += bi->start[bi->offset++];
        bi->field.integer -= '0';
    }

    if (bi->offset >= bi->count || bi->start[bi->offset++] != '\n') return false;

    bi->field.blob_start = NULL;
    if (bi->field.type == BI_BLOB) {
        bi->field.blob_start = &bi->start[bi->offset];
        bi->offset += bi->field.blob_count;
        if (bi->offset >= bi->count || bi->start[bi->offset++] != '\n') return false;
        return true;
    }

    return true;
}

#endif // BI_IMPLEMENTATION


#include "libVector.h"
#include <assert.h> // Using assert for simple test validation

// Implementations
static VectorStatus vector_ensure_capacity(Vector* vec, size_t requiredCapacity) {
    if (vec->capacity >= requiredCapacity) return VEC_OK;
    size_t new_capacity = vec->capacity == 0 ? 8 : vec->capacity * 2;
    if (new_capacity < requiredCapacity) new_capacity = requiredCapacity;
    void* new_items = realloc(vec->items, new_capacity * vec->itemSize);
    if (!new_items) return VEC_ERROR_MEMORY;
    vec->items = new_items;
    vec->capacity = new_capacity;
    return VEC_OK;
}
VectorStatus vector_init(Vector* vec, size_t itemSize) {
    if (!vec || itemSize == 0) return VEC_ERROR;
    vec->items = NULL;
    vec->count = 0;
    vec->capacity = 0;
    vec->itemSize = itemSize;
    return VEC_OK;
}
void vector_destroy(Vector* vec) {
    if (vec && vec->items) {
        free(vec->items);
    }
    // Correctly reset all members to a clean state after freeing.
    if (vec) {
        vec->items = NULL;
        vec->count = 0;
        vec->capacity = 0;
        vec->itemSize = 0; // Also reset itemSize
    }
}
VectorStatus vector_push_back(Vector* vec, const void* item) {
    if (vector_ensure_capacity(vec, vec->count + 1) != VEC_OK) return VEC_ERROR_MEMORY;
    char* dest = (char*)vec->items + vec->count * vec->itemSize;
    memcpy(dest, item, vec->itemSize);
    vec->count++;
    return VEC_OK;
}
VectorStatus vector_insert_ordered(Vector* vec, const void* item, CmpFunc cmp, UpdateFunc update) {
    size_t pos = 0;
    char* data = (char*)vec->items;
    while (pos < vec->count && cmp(data + pos * vec->itemSize, item) < 0) pos++;
    if (pos < vec->count && cmp(data + pos * vec->itemSize, item) == 0) {
        if (update) update(data + pos * vec->itemSize, item);
        return VEC_OK;
    }
    if (vector_ensure_capacity(vec, vec->count + 1) != VEC_OK) return VEC_ERROR_MEMORY;
    data = (char*)vec->items;
    char* insert_pos = data + pos * vec->itemSize;
    memmove(insert_pos + vec->itemSize, insert_pos, (vec->count - pos) * vec->itemSize);
    memcpy(insert_pos, item, vec->itemSize);
    vec->count++;
    return VEC_OK;
}
int vector_search_ordered(const Vector* vec, const void* key, CmpFunc cmp) {
    if (!vec || !vec->items) return VEC_NOT_FOUND;
    size_t low = 0, high = vec->count;
    char* data = (char*)vec->items;
    while (low < high) {
        size_t mid = low + (high - low) / 2;
        void* mid_item = data + mid * vec->itemSize;
        int cmp_res = cmp(key, mid_item);
        if (cmp_res == 0) return mid;
        else if (cmp_res < 0) high = mid;
        else low = mid + 1;
    }
    return VEC_NOT_FOUND;
}
VectorStatus vector_remove_duplicates_ordered(Vector* vec, CmpFunc cmp, UpdateFunc update) {
    if (vec->count < 2) return VEC_OK;
    size_t write_idx = 0;
    char* data = (char*)vec->items;
    for (size_t read_idx = 1; read_idx < vec->count; ++read_idx) {
        void* write_item = data + write_idx * vec->itemSize;
        void* read_item = data + read_idx * vec->itemSize;
        if (cmp(write_item, read_item) == 0) {
            if (update) update(write_item, read_item);
        } else {
            write_idx++;
            if (write_idx != read_idx) memcpy(data + write_idx * vec->itemSize, read_item, vec->itemSize);
        }
    }
    vec->count = write_idx + 1;
    return VEC_OK;
}
void* vector_get_item_at(const Vector* vec, size_t index) {
    if (!vec || index >= vec->count) return NULL;
    return (char*)vec->items + index * vec->itemSize;
}
void vector_map(const Vector* vec, ActionFunc action, void* userData) {
    if (!vec || !action) return;
    for (size_t i = 0; i < vec->count; ++i) {
        void* item = vector_get_item_at(vec, i);
        action(item, userData);
    }
}


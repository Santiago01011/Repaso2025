#ifndef LIBVECTOR_H
#define LIBVECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =================================================================
// TYPE DEFINITIONS
// =================================================================

// Using an enum for return codes makes the calling code more readable.
typedef enum {
    VEC_OK = 1,
    VEC_ERROR = 0,
    VEC_ERROR_MEMORY = -1,
    VEC_NOT_FOUND = -2
} VectorStatus;

typedef struct {
    void* items;       // Renamed 'vec' to 'items' for clarity
    size_t count;       // Renamed 'ce' to 'count'
    size_t capacity;    // Renamed 'cap' to 'capacity'
    size_t itemSize;    // Renamed 'tamElem' to 'itemSize'
} Vector;

// Function pointer definitions remain the same
typedef int (*CmpFunc)(const void* d1, const void* d2);
typedef void (*UpdateFunc)(void* toUpdate, const void* updater);
typedef void (*ActionFunc)(void* item, void* userData);


// =================================================================
// FUNCTION PROTOTYPES (The Public API)
// =================================================================

// --- Core Lifecycle Functions ---
VectorStatus vector_init(Vector* vec, size_t itemSize);
void vector_destroy(Vector* vec);

// --- Insertion Functions ---
VectorStatus vector_push_back(Vector* vec, const void* item);
VectorStatus vector_insert_ordered(Vector* vec, const void* item, CmpFunc cmp, UpdateFunc update);

// --- Deletion and Searching ---
VectorStatus vector_remove_duplicates_ordered(Vector* vec, CmpFunc cmp, UpdateFunc update);
int vector_search_ordered(const Vector* vec, const void* key, CmpFunc cmp);

// --- Access and Iteration ---
void* vector_get_item_at(const Vector* vec, size_t index);
void vector_map(const Vector* vec, ActionFunc action, void* userData);

// --- Utility Functions ---
int cmpCuenta(const void* a, const void* b);





#endif // LIBVECTOR_H
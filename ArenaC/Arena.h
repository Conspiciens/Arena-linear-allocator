#ifndef ARENA_H 
#define ARENA_H 

#include <assert.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <unistd.h> 
#include <limits.h> 
#include <sys/types.h> 
#include <sys/mman.h> 
#include <stddef.h> 
#include <stdbool.h> 

/* Unused struct */
typedef struct {
    void *ptr;
    size_t size; 
} FatPointer;  

typedef struct {
    void *ptr; 
    size_t capacity; 
    size_t prev_offset;
    size_t offset;
} Arena;  

#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

Arena alloc_arena(); 
bool is_aligned_memory(uintptr_t ptr_addr);
void resize_map(Arena *self);
uintptr_t align_forward(uintptr_t ptr, size_t align); 
void *push(Arena *self, size_t len, size_t alignment); 
void clear(Arena *self, size_t offset, size_t len);
void *arena_resize(Arena *self, void *old_mem, size_t prev_len, size_t len, size_t alignment);
void dealloc_arena(Arena *self);

#endif
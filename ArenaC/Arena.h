#ifndef ARENA_H 
#define ARENA_H 

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

#endif ARENA_H 
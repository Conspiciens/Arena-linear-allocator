#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <unistd.h> 
#include <limits.h> 
#include <sys/types.h> 
#include <sys/mman.h> 
#include <stddef.h> 
#include <stdbool.h> 

#include <assert.h>

/* https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/ */

/* 
   16 bytes is the default alignment, a ptr in a 64 bit system is 8 bytes 
*/ 
#ifndef DEFAULT_ALIGNMENT 
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif 

typedef struct {
    void *ptr; 
    size_t capacity; 
    size_t prev_offset;
    size_t offset;
} Arena;  

Arena alloc_arena() {
   int page_size = getpagesize(); 
   size_t page_len = (size_t)page_size; 

   if (page_size == -1) {
        printf("Error occured in sysconf"); 
        exit(0); 
   }  

   void *ptr = mmap(NULL, page_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
   if (ptr == MAP_FAILED) {  
        printf("Error occured in sysconf"); 
        exit(0); 
   } 

   return (Arena) {
        .ptr = ptr, 
        .capacity = page_size, 
        .offset = 0 
    };  
} 
 
bool is_aligned_memory(uintptr_t ptr_addr) {
    return (ptr_addr & (ptr_addr - 1)) == 0; 
} 

/* We move the pointer to the next aligned memory addr */ 
uintptr_t align_forward(uintptr_t ptr, size_t align) {
    // 16 - 2 * 2 * 2 * 2
    // 32 - 2 * 2 * 2 * 2 * 2

    // 17 % 2 == 1 

    assert(is_aligned_memory((uintptr_t)align) == true); 

    uintptr_t a = (uintptr_t)align; 
    uintptr_t modulo = (uintptr_t)ptr & (a - 1);  

    /* 
        I assume the size_t align means the byte boundary for the current architecture 
        
        align = 8  
        ptr = 13
        
        reminder = 5
    */ 
    
    if (modulo != 0) {
        perror("Issues resizing"); 
        // 16 - bytes left to align to the next alignment of 16 bytes 
        ptr += a - modulo;  
    } 

    /* return (ptr + align - 1) & ~(align - 1); Production type code */ 
    return ptr; 
} 

void* push(Arena *self, size_t len, size_t alignment) {
    uintptr_t curr_ptr_addr = (uintptr_t)self->ptr + (uintptr_t)self->offset;
    uintptr_t aligned_offset = align_forward(curr_ptr_addr, alignment); 

    // We get the ptr in the aligned offset, so we subtract (aligned_offset - self->ptr) to get the amount of bytes required to move 
    aligned_offset -= (uintptr_t)self->ptr; 

    printf("Aligned extra bytes: %lu\n", aligned_offset); 

    if (self->capacity <= self->offset + aligned_offset + len)    
        return NULL; 

    printf("Aligned extra bytes total: %lu\n", aligned_offset + len); 

    void *ptr = self->ptr + aligned_offset; 
    memset(ptr, 0, len); 

    self->offset = aligned_offset + len; 

    return ptr;  
} 

void pop(Arena *self) {} 

void *arena_resize(Arena *self, void *old_mem, size_t prev_len, size_t len, size_t alignment) {

    if (old_mem == NULL || prev_len == 0) {
        return push(self, len, alignment); 
    } else if (self->ptr <= old_mem && self->ptr + self->capacity > old_mem) {

        /* Checking whether the prev offset is equal to the old memory */ 
        if (self->ptr + self->prev_offset == old_mem) { 
            self->offset = self->prev_offset + len; 
        
            /* if the len is greater than the previous allocated memory than expand by the difference */ 
            if (len > prev_len)
                memset(self->ptr + self->offset, 0, len - prev_len); 

            return old_mem; 
       } else {
            void *new_mem = push(self, len, alignment); 
            size_t copy_size = prev_len < len ? prev_len : len; 
            
            memmove(new_mem, old_mem, copy_size); 
            return new_mem; 
       }  
    } else {
        perror("Issues resizing"); 
        return NULL;
    }  
} 

void dealloc_arena(Arena *self) {
    int flag = munmap(self->ptr, self->capacity); 
    if (flag == -1) {
        printf("Failed to dellocate\n"); 
        exit(0); 
    } 

    self->ptr = NULL; 
    self->offset = 0; 
    self->prev_offset = 0; 
    self->capacity = 0; 
} 

void tests() {

    // Test 1 - Allocation of first 7 bytes 
    Arena arena = alloc_arena(); 

    printf("Allocated Size: %zu \n", arena.offset); 

    assert(arena.offset == 0); 
    assert(arena.capacity == (size_t)getpagesize());

    printf("Memory location before pushing: %p \n", (void*)arena.ptr);
    void* ptr_offset = push(&arena, 7, DEFAULT_ALIGNMENT); 

    assert((uintptr_t)ptr_offset % DEFAULT_ALIGNMENT == 0); 


    printf("Memory location after pushing: %p \n", (void*)ptr_offset);
    printf("Allocated Size: %zu \n", arena.offset); 

    // Test 2 - Allocation of next 7 bytes
    printf("Memory location before pushing: %p \n", (void*)arena.ptr);
    void* ptr_2_offset = push(&arena, 7, DEFAULT_ALIGNMENT); 

    assert((uintptr_t)ptr_2_offset % DEFAULT_ALIGNMENT == 0); 

    printf("Memory location after pushing: %p \n", (void*)ptr_2_offset);
    printf("Allocated Size: %zu \n", arena.offset); 

    dealloc_arena(&arena); 
} 

void test_align_memory(Arena *self) {
    
} 

int main(void) {
    tests(); 
    return 0; 
} 

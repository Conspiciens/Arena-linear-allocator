#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <unistd.h> 
#include <limits.h> 
#include <sys/mman.h> 
#include <stddef.h> 
#include <stdbool.h> 

#include <assert.h>

/* https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/ */

/* 
    16 bytes as the size of bytes is 8 bytes, however this is our default alignment 
*/ 
#ifndef DEFAULT_ALIGNMENT 
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif 

typedef struct {
    void *ptr; 
    size_t capacity; 
    size_t offset;
} Arena;  

Arena alloc_aren() {
   int page_size = getpagesize(); 
   size_t page_len = (size_t)page_size; 

   if (page_size == -1) {
        printf("Error occured in sysconf"); 
        exit(0); 
   }  

   void *ptr = mmap(NULL, page_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
   if (ptr == MAP_FAILED) {  
        printf("Error occured in sysconf"); 
        exit(1); 
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

    printf("Memory address before alignment: %lu\n", (uintptr_t)ptr); 
    printf("Alignment: %lu\n", a); 

    uintptr_t modulo = (uintptr_t)ptr & (a - 1);  

    /* 
        I assume the size_t align means the byte boundary for the current architecture 
        
        align = 8  
        ptr = 13
        
        reminder = 5
    */ 
    
    printf("Modulo %lu\n", modulo); 
    if (modulo != 0) {
        ptr += a - modulo;  
    } 

    printf("Memory address after alignment: %lu\n", (uintptr_t)ptr); 

    /* return (ptr + align - 1) & ~(align - 1); Production type code */ 
    return ptr; 
} 

void* push(Arena *self, size_t len, size_t alignment) {
    uintptr_t curr_ptr_addr = (uintptr_t)self->ptr + (uintptr_t)self->offset;
    uintptr_t aligned_offset = align_forward(curr_ptr_addr, alignment); 

    aligned_offset -= (uintptr_t)self->ptr; 

    if (self->capacity <= self->offset + aligned_offset + len)    
        return NULL; 

    void *ptr = self->ptr + aligned_offset; 
    memset(ptr, 0, len); 

    printf("Memory location after pushing test: %p \n", (void*)ptr);

    self->offset += aligned_offset + len; 

    return ptr;  
} 

void pop(Arena *self) {} 

void dealloc_aren(Arena *self) {
    int flag = munmap(self->ptr, self->capacity); 
    if (flag == -1) {
        printf("Failed to dellocate\n"); 
        exit(1); 
    }  
} 

void tests() {
    Arena arena = alloc_aren(); 

    printf("Allocated Size: %zu \n", arena.offset); 

    assert(arena.offset == 0); 
    assert(arena.capacity == (size_t)getpagesize());

    printf("Memory location before pushing: %p \n", (void*)arena.ptr);
    void* ptr_offset = push(&arena, 10, DEFAULT_ALIGNMENT); 

    assert((uintptr_t)ptr_offset % DEFAULT_ALIGNMENT == 0); 
    printf("Is pwr 2: %d\n", is_pwr_2);


    printf("Memory location after pushing: %p \n", (void*)ptr_offset);
    printf("Allocated Size: %zu \n", arena.offset); 

    dealloc_aren(&arena); 
} 

void test_align_memory(Arena *self) {} 

int main(void) {
    tests(); 
    return 0; 
} 

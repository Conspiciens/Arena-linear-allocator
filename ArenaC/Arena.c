#include "Arena.h"

/* https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/ */

/* 
   16 bytes is the default alignment, a ptr in a 64 bit system is 8 bytes 
*/ 

Arena alloc_arena() {
   int page_size = getpagesize(); 
   size_t page_len = (size_t)page_size; 

   if (page_size == -1) {
        printf("Error occured in sysconf"); 
        exit(0); 
   }  

   void *ptr = mmap(
        NULL, 
        page_len, 
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 
        -1, 
        0
    ); 
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

void resize_map(Arena *self) {
   int page_size = getpagesize(); 
   size_t new_page_len = self->capacity + (size_t)page_size; 

   if (page_size == -1) {
        printf("Error occured in sysconf"); 
        exit(0); 
   }  

   void *new_ptr = mmap(NULL, new_page_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
   if (new_ptr == MAP_FAILED) {  
        printf("Error occured in sysconf"); 
        exit(0); 
   } 

   memmove(new_ptr, self->ptr, self->capacity); 

   self->ptr = new_ptr; 
   self->capacity = new_page_len; 
} 

/* We move the pointer to the next aligned memory addr */ 
uintptr_t align_forward(uintptr_t ptr, size_t align) {
    // assert(is_aligned_memory((uintptr_t)align) == true); 

    // uintptr_t a = (uintptr_t)align; 
    // uintptr_t modulo = (uintptr_t)ptr & (a - 1);  
    /* 
        Example of how the logic works is pointed down below

        Ex. 
            ptr = 1 
            alignment = 16 

            00001 & 01111 = 00001 = 1 
            16 - 1 = 15 

            ptr += 15
    */

    
    // if (modulo != 0) {
    //     // 16 - bytes left to align to the next alignment of 16 bytes 
    //     ptr += a - modulo;  
    // } 

    /* 
        ptr = 1 
        alignment = 16

        (1 + 15) & ~(15)

        But also any bits before 15 is inverted therfore, it's really a large number if size_t it's probably 64 bits 
        16 & 10000 = 1000 = 16, keeps any nums that are divisble by 16
    */


    return (ptr + align - 1) & ~(align - 1);  
    // return ptr; 
} 

void* push(Arena *self, size_t len, size_t alignment) {
    uintptr_t curr_ptr_addr = (uintptr_t)self->ptr + (uintptr_t)self->offset;
    uintptr_t aligned_offset = align_forward(curr_ptr_addr, alignment); 

    // We get the ptr in the aligned offset, so we subtract (aligned_offset - self->ptr) to get the amount of bytes required to move 
    aligned_offset -= (uintptr_t)self->ptr; 

    printf("Aligned extra bytes: %lu\n", aligned_offset); 

    if (self->capacity <= aligned_offset + len)    
        return NULL; 

    printf("Aligned extra bytes total: %lu\n", aligned_offset + len); 

    void *ptr = self->ptr + aligned_offset; 
    memset(ptr, 0, len); 

    self->offset = aligned_offset + len; 

    return ptr;  
} 

void clear(Arena *self, size_t offset, size_t len) {
    if ((uintptr_t)self->ptr + offset >= self->capacity) {
        return; 
    }

    memset(self->ptr + offset, 0, len - offset); 
} 

void *arena_resize(Arena *self, void *old_mem, size_t prev_len, size_t len, size_t alignment) {

    /* Check if prev mem has been initialized or prev_len*/
    if (old_mem == NULL || prev_len == 0) {
        return push(self, len, alignment); 
    /* if self->ptr is less thean the old_mem and is greater than the ptr  */
    } else if (self->ptr <= old_mem && self->ptr + self->capacity > old_mem) {

        /* Checking whether the prev offset is equal to the old memory */ 
        if (self->ptr + self->prev_offset == old_mem) { 
            self->offset = self->prev_offset + len; 
        
            /* 
                if the new length is greater than the previous 
                allocated memory than expand by the difference 
            */ 
            if (len > prev_len)
                memset(self->ptr + self->offset, 0, len - prev_len); 

            return old_mem; 
       } else {
            void *new_mem = push(self, len, alignment); 
            size_t copy_size = prev_len < len ? prev_len : len; 
            
            /* 
                Old memory is left behind and the new memory is updated with all the information from
                old memory    
            */
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



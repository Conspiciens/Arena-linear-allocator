#include "Arena.h"

void test_init() {
    Arena arena = alloc_arena(); 
    
    assert(arena.offset == 0); 
    assert(arena.capacity == (size_t)getpagesize()); 

    dealloc_arena(&arena); 
}

void test_alloc() {
    Arena arena = alloc_arena(); 

    void* ptr_offset = push(&arena, 7, DEFAULT_ALIGNMENT); 

    assert((uintptr_t)ptr_offset % DEFAULT_ALIGNMENT == 0); 

    printf("Memory location after after attempt: %p \n", (void*)ptr_offset);
    printf("Allocated Size: %zu \n", arena.offset); 

    // Test 2 - Allocation of next 7 bytes
    printf("Memory location before second attempt: %p \n", (void*)arena.ptr);
    void* ptr_2_offset = push(&arena, 7, DEFAULT_ALIGNMENT); 

    assert((uintptr_t)ptr_2_offset % DEFAULT_ALIGNMENT == 0); 

    printf("Memory location after second attempt: %p \n", (void*)ptr_2_offset);
    printf("Allocated Size: %zu \n", arena.offset); 

    printf("\n\n\n");

    dealloc_arena(&arena); 
}

void test_resize_prev_mem() { 
    Arena arena = alloc_arena(); 

    void* ptr = push(&arena, 7, DEFAULT_ALIGNMENT); 
    void* resized_ptr = arena_resize(&arena, ptr, 7, 32, DEFAULT_ALIGNMENT);

    printf("Resize ptr: %p", resized_ptr);

    assert(resized_ptr != NULL);
    assert((uintptr_t)ptr == (uintptr_t)resized_ptr); 

    dealloc_arena(&arena); 
}

void test_resize_prev_mem_copy() {
    Arena arena = alloc_arena(); 

    void *ptr = push(&arena, 7, DEFAULT_ALIGNMENT); 

    void *ptr_update = (char *)ptr + 3;
    *(int *)ptr_update = 4; 

    void *ptr2 = push(&arena, 7, DEFAULT_ALIGNMENT); 

    void *resized_ptr = arena_resize(&arena, ptr, 7, 32, DEFAULT_ALIGNMENT); 
    resized_ptr = (char *)resized_ptr + 3; 
    assert(4 == *(int *)resized_ptr); 

    dealloc_arena(&arena);
}


int main() {
    test_init();
    test_alloc(); 
    test_resize_prev_mem();  
    test_resize_prev_mem_copy();
    return 0;
}

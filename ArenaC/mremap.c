#include <mach/mach.h> 
#include <mach/mach_vm.h> 
#include <stdio.h> 
#include <stdbool.h> 

bool is_able_to_increase_size(Arena *self, void *ptr, size_t offset, size_t len) {

    task_t port = mach_task_self(); 
    vm_address_t target_addr = (uintptr_t)ptr + offset; 
    kern_return_t err = vm_allocate(
        port,  
        &target_addr, 
        len, 
        FALSE
    ); 

    if (err == -1)
        return -1;
    

    return 0; 
}

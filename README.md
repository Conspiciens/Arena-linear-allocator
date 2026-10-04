## <ins> Linear Arena Implmentation (In C & Rust) <ins>
Implemented a simple Arena allocator in C/Rust with alignment (16 bytes alignement), with basic functionality
with the assistance of https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/ 


> Note: Rust returns Fat Pointer to ensure there's a known size to compare

ArenaC 
    - Arena.c         
ArenaRust 
    - ArenaRust
        - lib.rs
        - sysgetconf.rs 


They have most of the same functions
    - alloc_arena/new 
    - resize 
    - alignment 
    - push/allocate_mem
    - dealloc/drop
        

## <ins> Reason for default alignment 16 bytes <ins>
16 bytes allows for SIMD (Single Intruction Multiple Data), which are listed below

XMM registers are part of SSE (Streaming SIMD Extension) 
- XMM Registers 
    - 8 XMM registers non -64-bit modes 
    - 16 XMM registers in long mode (simultaneous)
        - 16 bytes
        - eight words 
        - four double words 
        - two quad words
        - four floats 
        - two doulbes 

The equivalent in ARM is Neon        

Also consider that the cache line reads every 64 bytes (We want to find within that cache line)


## <ins> Functions used <ins>
mmap() 
- creates a new mapping in the virtual address space in the calling process 

- Page-aligned address for addr

- Macos: 16 kilobyte page
- linux: 4 kilobyte page

- MAP_ANONYMOUS
    - offset must be a multiple of the page_size
    - initialized using length bytes starting at the 
    - offset in the file

            
    
    
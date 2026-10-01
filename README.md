# Linear Arena Implmentation (In C & Rust)
    Implemented a simple Arena allocator in C with some alignment (16 bytes alignement)
    All Implementations used mmap for memory allocation

    Arena.c 
        

    ArenaRust 
        ArenaRust
            lib.rs
            sysgetconf.rs 
            main.rs

# Reason for default alignment 16 bytes 
    16 bytes allows for SIMD (Single Intruction Multiple Data), which are listed below

    XMM registers are part of SSE (Streaming SIMD Extension) 
    XMM Registers 
        8 XMM registers non -64-bit modes 
        16 XMM registers in long mode (simultaneous)
            16 bytes
            eight words 
            four double words 
            two quad words
            four floats 
            two doulbes 

    The equivalent in ARM is Neon        

    Also consider that the cache line reads every 64 bytes (We want to find within that cache line)

# Functions used 
    mmap() 
        creates a new mapping in the virtual address space in the caling process 
        
        Page-aligned address for addr

        MAP_ANONYMOUS
            offset must be a multiple of the page_size

            initialized using length bytes starting at the 
            offset in the file

            

    
# Arena Implmentation (In C, hopefully Rust implementation following soon)
    Implemented a simple Arena allocator in C with some alignment (2, 4, 8, 16, 32, etc)

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

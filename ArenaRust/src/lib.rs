use std::ptr; 
use std::mem; 
use memmap2::MmapMut; 
use libc::{mmap, munmap, MAP_SHARED, PROT_READ, PROT_WRITE, MAP_ANONYMOUS}; 

mod sysgetconf;  
mod dyn_mem;

const DEFAULT_ALIGNMENT: usize = std::mem
    ::size_of::<*const u8>() * 2; 

struct Arena {
    ptr: *mut u8, 
    offset: usize, 
    capacity: usize, 
} 

impl Arena {
    pub fn new() -> Self {
        let page_size = sysgetconf::get_page_size(); 

        let mut mut_ptr = unsafe {
            mmap(
                ptr::null_mut(), 
                page_size,  
                PROT_READ | PROT_WRITE, 
                MAP_SHARED | MAP_ANONYMOUS, 
                -1, 
                0  
            ) 
        }; 

        if mut_ptr == libc::MAP_FAILED {
            panic!("Map failed to be created"); 
        } 

        Arena {
            ptr: mut_ptr as *mut u8,  
            offset: 0, 
            capacity: page_size,
        }
    } 

    pub fn allocate_mem(&mut self, len: usize) -> Option<&mut [u8]> {
        if self.offset + len > self.capacity {
            return None
        } 
    
        let mut mut_ptr = unsafe {
            std::slice::from_raw_parts_mut(self.ptr.add(self.offset), len)
        };     

        self.offset += len; 
        Some(mut_ptr)
    } 


    pub fn alignment(ptr: *mut u8, alignment: usize) -> *mut u8{
       let mut ptr_addr = ptr as usize; 
       println!("ptr mem: {:?}", ptr_addr); 
       let a = alignment; 
       let modulo = ptr_addr & (a - 1);  

       println!("Modulo: {}", modulo);

       if modulo != 0 {
            ptr_addr += a - modulo; 
       } 
       println!("ptr mem: {:?}", ptr_addr); 

       unsafe {
           return ptr_addr as *mut u8; 
       }
    } 
} 


#[cfg(test)]
mod tests {
    use super::*;
    use sysgetconf;  

    #[test] 
    fn default_alignment() {
        assert_eq!(DEFAULT_ALIGNMENT, 16, "Default Alignment is 16 bytes for ARM64"); 
    } 

    #[test]
    fn test_mem_alignment() {
        let mut arena = Arena::new(); 
        let first_ptr = arena.allocate_mem(7).unwrap(); 
        let sec_ptr = arena.allocate_mem(7).unwrap();

        let mem_size = Arena::alignment(sec_ptr.as_mut_ptr(), DEFAULT_ALIGNMENT); 
        println!("Mem Ptr: {:?}", mem_size); 
        println!("Sec Ptr: {:?}", sec_ptr.as_ptr()); 

        let offset = unsafe {
            (mem_size as *const u8).offset_from((sec_ptr.as_ptr() as *const u8))
        };

        println!("Offset: {}", offset); 

        assert_eq!(offset, 9, "Alignment should add an additional 8 bytes"); 
    } 

    #[test] 
    fn test_if_capacity_maxed() {
        let mut arena = Arena::new(); 

        let opt_ptr = arena.allocate_mem(16384); 
        let mut mut_ptr = opt_ptr.unwrap(); 

        let sec_ptr = arena.allocate_mem(8); 

        assert_eq!(Some(sec_ptr), Some(None), "Unable to use memory allocated, since the page is full!"); 
    } 

    #[test]
    fn alloc_arena() {
        let mut arena = Arena::new(); 
        let opt_ptr = arena.allocate_mem(8); 
        
        let mut ptr = opt_ptr.unwrap(); 
        assert_eq!(ptr.len(), 8, "Slice should be equal to memory allocated"); 
    }
}

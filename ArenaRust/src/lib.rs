use std::ptr; 
use std::mem; 
use memmap2::MmapMut; 
use libc::{mmap, munmap, MAP_SHARED, PROT_READ, PROT_WRITE, MAP_ANONYMOUS}; 

mod sysgetconf;  
mod dyn_mem;

const DEFAULT_ALIGNMENT: usize = std::mem
    ::size_of::<*const u8>() * 2; 

struct Arena {
    ptr: *mut usize, 
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
            ptr: mut_ptr as *mut usize,  
            offset: 0, 
            capacity: page_size,
        }
    } 

    pub fn allocate_mem(&mut self, len: usize) -> Option<&mut [usize]> {
        if self.offset + len > self.capacity {
            return None
        } 

        let mut mut_ptr; 
        unsafe {
            mut_ptr = std::slice::from_raw_parts_mut(self.ptr, len); 
        }        

        self.offset += len; 
        Some(mut_ptr)
    } 

    pub fn alignment(ptr: *mut usize, alignment: usize) -> *mut usize {
       let mut ptr_mem = ptr as usize; 
       let a = alignment; 
       let modulo = ptr_mem & (a - 1);  

       println!("Modulo: {}", modulo);

       if modulo != 0 {
            ptr_mem += a - modulo; 
       } 

       return ptr; 
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
        let opt_ptr = arena.allocate_mem(7); 
        let mut mut_ptr = opt_ptr.unwrap(); 

        let sec_opt_ptr = arena.allocate_mem(7).unwrap(); 

        let mem_size = Arena::alignment(sec_opt_ptr.as_mut_ptr(), DEFAULT_ALIGNMENT); 
        println!("Mem size: {:?}", mem_size); 
        println!("Mut Ptr: {:?}", sec_opt_ptr); 
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

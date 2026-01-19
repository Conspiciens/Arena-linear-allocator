mod sysgetconf;  

use std::ptr; 
use std::mem; 
use memmap2::MmapMut; 

const DEFAULT_ALIGNMENT: usize = std::mem
    ::size_of::<*const u8>() * 2; 

struct Arena {
    ptr: MmapMut, 
    offset: usize, 
    capacity: usize, 
} 

impl Arena {
    pub fn new() -> Self {
        let page_size = sysgetconf::get_page_size(); 
        let map = MmapMut::map_anon(page_size); 

        let mmap_mut = match map {
            Ok(mmapMut) => mmapMut, 
            Err(err) => panic!("Error {}", err), 
        }; 
    
        let capacity = mmap_mut.len(); 

        Arena {
            ptr: mmap_mut, 
            offset: 0, 
            capacity: capacity,
        }
    } 

    pub fn allocate_mem(&mut self, len: usize) -> Option<&mut [u8]>{
        if let Some(mut_ptr) = self.ptr.get_mut(self.offset..len) {
            self.offset += len; 
            return Some(mut_ptr); 
        } 
        None
    } 

    pub fn alignment(mut ptr: usize, alignment: usize) -> usize {
       let a = alignment; 
       let modulo = ptr & (a - 1);  

       println!("Modulo: {}", modulo);

       if modulo != 0 {
            ptr += a - modulo; 
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
        let mut mut_ptr = match opt_ptr {
            Some(mut_ptr) => mut_ptr.as_mut_ptr(),
            None => panic!("Error occured when getting ptr"), 
        }; 

        let sec_opt_ptr = arena.allocate_mem(7); 
        let mut sec_mut_ptr = match sec_opt_ptr {
            Some(mut_ptr) => mut_ptr.as_mut_ptr(),
            None => panic!("Error occured when getting ptr"), 
        }; 

        let mem_size = Arena::alignment(sec_mut_ptr as usize, DEFAULT_ALIGNMENT); 
        let total_mem_size = mem_size - (sec_mut_ptr as usize); 
        println!("Mem size: {}", mem_size); 
        println!("Mut Ptr: {}", sec_mut_ptr as usize); 
        assert_eq!(total_mem_size, 9, "Memory should be allocated to 9 bytes as the default alignment is 16!"); 
        
    } 

    #[test]
    fn init_Arena() {
        let arena = Arena::new(); 
        println!("Arena: {}", arena.capacity); 

        /* Mac OS ARM is 16384 bytes */ 
        assert_eq!(arena.capacity, sysgetconf::get_page_size(), "Mac OS ARM page is 16384 bytes" ); 
        assert_eq!(arena.ptr.len(), sysgetconf::get_page_size(), "Check arena ptr for page allocation"); 
    }

    #[test]
    fn alloc_Arena() {
        let mut arena = Arena::new(); 

        let opt_ptr = arena.allocate_mem(7); 
        let mut mut_ptr = match opt_ptr {
            Some(mut_ptr) => mut_ptr.as_mut_ptr(),
            None => panic!("Error occured when getting ptr"), 
        }; 
        println!("Ptr: {:?}", mut_ptr); 

        let sec_opt_ptr = arena.allocate_mem(7); 
        let mut sec_mut_ptr = match sec_opt_ptr {
            Some(sec_mut_ptr) => sec_mut_ptr.as_mut_ptr(), 
            None => panic!("Error occured when getting ptr 2"), 
        }; 
        println!("Ptr: {:?}", sec_mut_ptr); 
       
    } 

}

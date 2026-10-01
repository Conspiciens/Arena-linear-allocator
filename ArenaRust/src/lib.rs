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
    prev_offset: usize, 
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
        let cur_ptr = self.ptr as usize + self.offset;  
        let align_ptr = Arena::alignment(cur_ptr as *mut u8, DEFAULT_ALIGNMENT);         

        let bytes_size: usize = unsafe { align_ptr.offset_from(cur_ptr as *const u8) as usize }; 

        if bytes_size + len > self.capacity {
            return None
        } 
    
        let mut mut_ptr = unsafe {
            std::slice::from_raw_parts_mut(self.ptr.add(bytes_size), len)
        };     

        self.offset = bytes_size + len; 
        Some(mut_ptr)
    } 


    pub fn alignment(
        ptr: *mut u8, 
        alignment: usize
    ) -> *mut u8 {
       let mut ptr_addr = ptr as usize; 
       let a = alignment; 
       let modulo = ptr_addr & (a - 1);  

       if modulo != 0 {
            ptr_addr += a - modulo; 
       } 

       return unsafe {
           ptr_addr as *mut u8
       }
    } 

    pub fn arena_resize(
        &mut self, 
        old_mem_ptr: *mut u8, 
        prev_size: usize,
        size: usize
    ) -> Option<*mut [u8]> {
        let curr_ptr = self.ptr as usize; 
        let prev_mem_ptr = old_mem_ptr as usize;

        if curr_ptr + self.offset + size < self.capacity {
            return self.allocate_mem(size)
        } else if curr_ptr <= prev_mem_ptr && curr_ptr + self.capacity > prev_mem_ptr { 
            
            if curr_ptr + self.prev_offset == prev_mem_ptr {
                /* Start from the previous offset, since it's unused */ 
                self.offset = self.prev_offset + size; 
            
                /* Check which size is bigger, if new size is bigger than prev size then we want to gain the difference space */ 
                if prev_size < size {
                    unsafe {
                        std::ptr::write_bytes(self.ptr.add(self.offset), 0, size - prev_size);
                    }
                } 
                    
                let old_mem_fat_ptr = unsafe { std::ptr::slice_from_raw_parts_mut(old_mem_ptr, size) };

                return Some(old_mem_fat_ptr)
            } else {
                /* Convert to raw pointer/thin pointer to update the size */ 
                let ptr = match self.allocate_mem(size) {
                    Some(ptr) => ptr as *mut u8, 
                    None => panic!("Failed to allocate mem")
                };

                
                
                /* Choose the largest size between the current and prev size */ 
                let largest_size = if size > prev_size {size} else {prev_size}; 
               
                /* New Fat Pointer is created */ 
                let new_fat_ptr = unsafe {
                    std::ptr::copy(old_mem_ptr, ptr, largest_size);
                    std::ptr::slice_from_raw_parts_mut(ptr, largest_size)
                };


                return Some(new_fat_ptr)
            }
            
        }

        return None; 
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

        assert_eq!(offset, 9, "Alignment should add an additional 9 bytes"); 
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

mod sysgetconf;  

use std::ptr; 
use memmap2::MmapMut; 

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
    

        Arena {
            ptr: mmap_mut, 
            offset: 0, 
            capacity: 0,
        }
    } 
} 

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn init_Arena() {
        let arena = Arena::new(); 
        assert!(arena.offset == 0); 
    }
}

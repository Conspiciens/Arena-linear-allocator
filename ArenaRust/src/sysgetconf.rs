extern crate libc; 

pub fn get_page_size() -> usize {
    unsafe {
        return libc::sysconf(libc::_SC_PAGESIZE) as usize; 
    } 
} 

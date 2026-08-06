use std::fs;
use std::path::Path;

use std::ffi::{CStr , CString , c_str , c_char};

#[repr(C)]
pub struct File_path_str {
    pub paths : *mut *mut c_char,
    pub length: usize ,
    pub capacity:usize,
}



#[unsafe(no_mangle)]
pub unsafe extern "C" fn file_find(dir_ptr: *const c_char , exit_ptr: *const c_char )-> File_path_str{


    if dir_ptr.is_null() || exit_ptr.is_null(){
        return File_path_str{ paths: std::ptr::null_mut() , length:0 , capacity:0};
    }

    let raw_dir = CStr::from_ptr(dir_ptr).to_string_lossy().trim().to_string();
    let file_type = CStr::from_ptr(exit_ptr).to_string_lossy().trim().to_string();


    let mut target = format!("./{}", raw_dir.trim_start_matches('/'));

    let mut found_path: Vec<*mut c_char> = Vec::new();
    

   

    if Path::new(&target).is_dir(){

        if let Ok(entries) = fs::read_dir(&target){

            for entry in entries.flatten(){
                let path = entry.path();
                if path.is_file() && path.extension().and_then(|s| s.to_str()) == Some(&file_type){
                    //converting the path to null terminated c_str
                    if let Some(path_str) = path.to_str(){
                        if let Ok(my_c_string)= CString::new(path_str){
                            found_path.push(my_c_string.into_raw());
                        }
                    }
                }
            }
        }

    }else{
        println!("no such dir found");
    }

    let length = found_path.len();
    let capacity = found_path.capacity();
    let mut boxed_slice = found_path.into_boxed_slice();
    let paths = boxed_slice.as_mut_ptr();
    std::mem::forget(boxed_slice);

    File_path_str {paths , length , capacity}

    

}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn free_file_paths(result: File_path_str){
    if result.paths.is_null(){return;}

    let path_vec = Vec::from_raw_parts(result.paths , result.length , result.capacity);

    for ptr in path_vec {
        if !ptr.is_null(){
            let _ = CString::from_raw(ptr);
        }
    }
}

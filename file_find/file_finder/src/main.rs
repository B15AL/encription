use std::fs;
use std::io;
use std::path::Path;


fn main()-> io::Result<()>{

    let mut target = String::new();
    let mut file_type = String::new();
    println!("write the dir name , example txt");

    io::stdin().read_line(&mut target).expect("file type reading failed");

    target.insert(0 , '/');
    target.insert(0, '.');
    let target = target.trim();


    println!("write the type of file you want , example txt");
    io::stdin().read_line(&mut file_type).expect("failed to read the file");

    let file_type = file_type.trim();//borrwoing string to &str for compare and also trimming \n

    

    if Path::new(target).is_dir(){
        

        for entry in fs::read_dir(target)?{
            let entry = entry?;
            let path = entry.path();

            if path.is_file() && path.extension().and_then(|s| s.to_str())==Some(file_type){
                println!("found file log: {:?}" , path);
            } 
        }

    }else{
        println!("no such dir found");
    }

    Ok(())

}

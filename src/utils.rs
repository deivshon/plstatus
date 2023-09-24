use std::process;

pub fn warn(msg: String) {
    eprintln!("Warning: {}", msg);
}

pub fn failure(msg: String) -> ! {
    eprintln!("Failure: {}", msg);
    process::exit(1);
}

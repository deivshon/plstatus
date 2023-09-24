mod component;

use std::{
    sync::{Arc, Mutex},
    thread,
    time::Duration,
};

use component::Component;

fn status_loop(status: Arc<Mutex<String>>, components: Vec<Arc<Mutex<Component>>>) {
    let mut status = status.lock().unwrap();
    loop {
        *status = String::from("");
        for c in &components {
            let component = c.lock().unwrap();
            *status = format!("{}|{}", *status, component.current_result);
        }
        println!("{}", status);

        thread::sleep(Duration::from_millis(25));
    }
}

fn main() {
    let status = Arc::new(Mutex::new(String::new()));
    let a = Arc::new(Mutex::new(Component::new(
        String::from("date"),
        vec![String::from("+%3N")],
        Some(100),
    )));

    let b = Arc::new(Mutex::new(Component::new(
        String::from("date"),
        vec![String::from("+%3N")],
        Some(50),
    )));

    let a_clone = Arc::clone(&a);
    let _a_handle = thread::spawn(move || Component::main_loop(&a_clone));

    let b_clone = Arc::clone(&b);
    let _b_handle = thread::spawn(move || Component::main_loop(&b_clone));

    let components_refs = vec![Arc::clone(&a), Arc::clone(&b)];
    let status_loop_handle = thread::spawn(|| status_loop(status, components_refs));

    status_loop_handle.join().unwrap();
}

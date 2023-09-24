use std::{
    process::Command,
    sync::{Arc, Mutex},
    thread,
    time::Duration,
};

pub struct Component {
    pub binary: String,
    pub args: Vec<String>,
    pub period: Option<u64>,
    pub current_result: String,
    pub separator: String,
}

impl Component {
    pub fn new(binary: String, args: Vec<String>, period: Option<u64>, separator: String) -> Self {
        return Component {
            binary,
            args,
            period,
            current_result: String::new(),
            separator,
        };
    }

    pub fn main_loop(component_ref: &Arc<Mutex<Self>>) {
        let period;
        {
            let component = component_ref.lock().unwrap();
            period = component.period;
        }

        loop {
            Self::run(component_ref);

            match period {
                Some(p) => thread::sleep(Duration::from_millis(p)),
                None => break,
            }
        }
    }

    fn run(component_ref: &Arc<Mutex<Self>>) {
        let mut command;
        {
            let component = component_ref.lock().unwrap();
            command = Command::new(&component.binary);
            let mut arged_command = &mut command;
            for string in &component.args {
                arged_command = arged_command.arg(string);
            }
        }

        let output = match command.output() {
            Ok(o) => String::from_utf8_lossy(&o.stdout).trim().to_string(),
            Err(_) => String::from("Error"),
        };

        {
            let mut component = component_ref.lock().unwrap();
            component.current_result = output;
        }
    }
}

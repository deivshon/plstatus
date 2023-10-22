use std::{
    io,
    process::Command,
    sync::{Arc, Mutex},
    thread,
    time::Duration,
};

pub struct Component {
    pub binary: String,
    pub args: Vec<String>,
    pub period: Option<u64>,
    pub stdout: String,
    pub stderr: String,
    pub status: io::Result<Option<i32>>,
    pub separator: String,
    pub label: String,
}

impl Component {
    pub fn new(
        binary: String,
        args: Vec<String>,
        period: Option<u64>,
        separator: String,
        label: String,
    ) -> Self {
        return Component {
            binary,
            args,
            period,
            stdout: String::new(),
            stderr: String::new(),
            status: Ok(Some(0)),
            separator,
            label,
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
            Ok(o) => o,
            Err(e) => {
                let mut component = component_ref.lock().unwrap();
                component.stdout = String::new();
                component.stderr = String::new();
                component.status = Err(e);
                return;
            }
        };

        {
            let mut component = component_ref.lock().unwrap();
            component.stdout = String::from_utf8_lossy(&output.stdout).trim().to_string();
            component.stderr = String::from_utf8_lossy(&output.stderr).trim().to_string();
            component.status = Ok(output.status.code())
        }
    }
}

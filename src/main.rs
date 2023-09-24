mod component;
mod config;
mod utils;

use std::{
    env,
    process::Command,
    sync::{Arc, Mutex},
    thread,
    time::Duration,
};

use argparse::{ArgumentParser, StoreOption};
use component::Component;
use config::{Config, DEFAULT_CONFIG_PATH};
use utils::{failure, warn};

fn status_loop(status: Arc<Mutex<String>>, config: Config) {
    if let Some(wait) = config.first_wait {
        thread::sleep(Duration::from_millis(wait));
    }

    let mut status = status.lock().unwrap();
    loop {
        (*status).clear();
        for (idx, c) in (&config.components).iter().enumerate() {
            let component = c.lock().unwrap();
            *status = format!(
                "{}{}{}{}",
                component.label,
                *status,
                component.current_result,
                if idx != config.components.len() - 1 && !component.current_result.is_empty() {
                    component.separator.as_str()
                } else {
                    ""
                },
            );
        }

        let print_status = Command::new("xsetroot")
            .arg("-name")
            .arg((*status).as_str())
            .status();

        if let Err(e) = print_status {
            warn(format!("Could not print status on bar: {}", e))
        }

        thread::sleep(Duration::from_millis(config.period));
    }
}

fn main() {
    let mut config_path: Option<String> = None;
    let config_option_help = format!(
        "Path to the configuration file (default: $HOME/{})",
        DEFAULT_CONFIG_PATH
    );

    {
        let mut ap = ArgumentParser::new();

        ap.refer(&mut config_path).add_option(
            &["-c", "--config"],
            StoreOption,
            &config_option_help,
        );

        ap.parse_args_or_exit();
    }

    let config_path = match config_path {
        Some(c) => c,
        None => match env::var("HOME") {
            Ok(h) => format!("{}/{}", h, DEFAULT_CONFIG_PATH),
            Err(_) => failure("Could not obtain default config path".to_string()),
        },
    };

    let config = Config::new(config_path);

    let status = Arc::new(Mutex::new(String::new()));
    for c in &config.components {
        let c_ref = Arc::clone(&c);
        thread::spawn(move || Component::main_loop(&c_ref));
    }

    let status_loop_handle = thread::spawn(move || status_loop(status, config));

    status_loop_handle.join().unwrap();
}

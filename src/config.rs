use std::{
    fs,
    path::Path,
    sync::{Arc, Mutex},
};

use crate::{component::Component, utils::failure};

pub const DEFAULT_CONFIG_PATH: &str = ".config/plstatus/config.json";
const DEFAULT_SEPARATOR: &str = "|";

#[derive(serde::Deserialize)]
#[serde(deny_unknown_fields)]
struct ConfigComponent {
    pub binary: String,
    pub arguments: Option<Vec<String>>,
    pub period: Option<u64>,
    pub separator: Option<String>,
    pub label: Option<String>,
}

#[derive(serde::Deserialize)]
#[serde(deny_unknown_fields)]
struct RawConfig {
    components: Vec<ConfigComponent>,
    period: Option<u64>,
    first_wait: Option<u64>,
}

impl RawConfig {
    fn get(config_path: String) -> Self {
        let config_content = match fs::read_to_string(Path::new(&config_path)) {
            Ok(c) => c,
            Err(e) => failure(format!("Could not open config at: {}: {}", config_path, e)),
        };

        match serde_json::from_str(&config_content) {
            Ok(c) => return c,
            Err(e) => failure(format!("Error in JSON config at: {}: {}", config_path, e)),
        }
    }
}

pub struct Config {
    pub components: Vec<Arc<Mutex<Component>>>,
    pub period: u64,
    pub first_wait: Option<u64>,
}

impl Config {
    pub fn new(config_path: String) -> Self {
        let config = RawConfig::get(config_path);
        let components = config
            .components
            .iter()
            .map(|c| {
                Arc::new(Mutex::new(Component::new(
                    c.binary.clone(),
                    match &c.arguments {
                        Some(a) => a.clone(),
                        None => vec![],
                    },
                    c.period,
                    match &c.separator {
                        Some(s) => s.clone(),
                        None => String::from(DEFAULT_SEPARATOR),
                    },
                    match &c.label {
                        Some(l) => l.clone(),
                        None => String::new(),
                    },
                )))
            })
            .collect::<Vec<Arc<Mutex<Component>>>>();
        let period = match config.period {
            Some(p) => p,
            None => 1000,
        };

        return Config {
            components,
            period,
            first_wait: config.first_wait,
        };
    }
}

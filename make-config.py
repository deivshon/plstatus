import json
import sys
import os

O = "{"
C = "}"

def load_config(configPath):
    with open(configPath, "r") as f:
        config = json.loads(f.read())
    
    return config

def failure(err):
    print(f"Error: {err}", file = sys.stderr)
    sys.exit(1)

def build_component(component):
    command = component["command"]
    period = component["period"]
    component_string = f"\t{O} \"{command}\", {O}\"{command}\", "

    for arg in component["args"]:
        component_string += f"\"{arg}\", "
    
    component_string += f"NULL{C}, "

    component_string += f"{period}{C},"
    return component_string + "\n"

build_separator = lambda s: f"\tseparator(\"{s}\"),\n"

DEFAULT_CONFIG = "config.def.json"
CONFIG = "config.json"

if os.path.isfile(CONFIG):
    config = load_config(CONFIG)
else:
    failure("No config file found")

configString = \
"""#include <stddef.h>
#include "plstatus.h"

"""

configString += f"#define MAX_LEN {config['max-status-length']}\n"
configString += f"#define UPDATE_PERIOD {config['update-period']}\n"

configString += \
"""
#define separator(str) {"printf", {"printf", str, NULL}, __UINT32_MAX__}

Component components[] = {
"""

for component in config["blocks"]:
    if component == "sep":
        if "sep" not in config:
            failure("Separator block found but no separator definition given")
        else:
            configString += build_separator(config["sep"])
    else:
        configString += build_component(component)

configString += f"\tNULL\n{C};\n"

with open("config.h", "w") as f:
    f.write(configString)

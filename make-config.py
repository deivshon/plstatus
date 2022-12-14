import json
import sys
import os

O = "{"
C = "}"

CONFIG = "config.json"

COMMAND = "command"
ARGUMENTS = "args"
PERIOD = "period"

SEPARATOR = "sep"

MAX_STATUS_LENGTH = "max-status-length"
UPDATE_PERIOD = "update-period"
BLOCKS = "blocks"

CONFIG_NEEDED = (MAX_STATUS_LENGTH, UPDATE_PERIOD, BLOCKS)
COMPONENT_NEEDED = (COMMAND, PERIOD)

def load_config(configPath):
    with open(configPath, "r") as f:
        try:
            config = json.loads(f.read())
        except:
            failure("\tCould not load JSON config file\n\tRemember to remove trailing commas and enclose all property fields in double quotes\n\tCheck where the parsing went wrong at https://jsonlint.com/")

    return config

def failure(err):
    print(f"Error: {err}", file = sys.stderr)
    sys.exit(1)

def build_component(component):
    # Check component validity
    for field in COMPONENT_NEEDED:
        if field not in component:
            failure(f"Needed field \"{field}\" not found in component:\n{json.dumps(component, indent = 2)}")

    command = component[COMMAND]
    period = component[PERIOD]

    component_string = f"\t{O} \"{command}\", {O} \"{command}\", "

    if ARGUMENTS in component:
        if isinstance(component[ARGUMENTS], str):
            component_string += f"\"{component[ARGUMENTS]}\", "
        else:
            for arg in component[ARGUMENTS]:
                component_string += f"\"{arg}\", "

    component_string += f"NULL {C}, {period} {C},\n"

    return component_string

build_separator = lambda s: f"\tseparator(\"{s}\"),\n"

if os.path.isfile(CONFIG):
    config = load_config(CONFIG)
else:
    failure("No config file found")

for field in CONFIG_NEEDED:
    if field not in config:
            failure(f"Needed field \"{field}\" not found in config")

configString = \
"""#include <stddef.h>
#include "plstatus.h"

"""

configString += f"#define MAX_LEN {config[MAX_STATUS_LENGTH]}\n"
configString += f"#define UPDATE_PERIOD {config[UPDATE_PERIOD]}\n"

configString += \
"""
#define separator(str) { "printf", { "printf", str, NULL}, __UINT32_MAX__ }

Component components[] = {
"""

for component in config["blocks"]:
    if component == SEPARATOR:
        if SEPARATOR not in config:
            failure("Separator block found but no separator definition given")
        else:
            configString += build_separator(config[SEPARATOR])
    else:
        configString += build_component(component)

configString += f"\tNULL\n{C};\n"

with open("config.h", "w") as f:
    f.write(configString)

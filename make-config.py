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


def failure(err):
    print(f"Error: {err}", file = sys.stderr)
    sys.exit(1)

def get_command_path(command):
    command = os.path.expanduser(command)

    # Command was given in the form of a path to file
    if command == os.path.abspath(command):
        if os.path.isfile(command):
            return command
        else:
            failure(f"Command \"{command}\" does not exist")

    # Command was given in the form of the name of the destination file,
    # so it needs to be searched within PATH
    for path in os.environ["PATH"].split(":"):
        if path == "": continue

        if os.path.isfile(path + f"/{command}"):
            return os.path.join(path, command)

    failure(f"Command \"{command}\" does not exist within any of the folders in PATH")

def load_config(configPath):
    with open(configPath, "r") as f:
        try:
            config = json.loads(f.read())
        except:
            failure("\tCould not load JSON config file\n\tRemember to remove trailing commas and enclose all property fields in double quotes\n\tCheck where the parsing went wrong at https://jsonlint.com/")

    return config

def build_component(component):
    # Check component validity
    for field in COMPONENT_NEEDED:
        if field not in component:
            failure(f"Needed field \"{field}\" not found in component:\n{json.dumps(component, indent = 2)}")

    command = get_command_path(component[COMMAND])
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


####################
# Load config.json #
####################


if os.path.isfile(CONFIG):
    config = load_config(CONFIG)
else:
    failure("No config file found")

for field in CONFIG_NEEDED:
    if field not in config:
            failure(f"Needed field \"{field}\" not found in config")


###########################
# Create config.h content #
###########################


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


#####################
# Write to config.h #
#####################


with open("config.h", "w") as f:
    f.write(configString)

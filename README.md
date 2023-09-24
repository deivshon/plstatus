# **plstatus**

plstatus (parallel status) is a minimal, JSON configurable status bar for [dwm](https://dwm.suckless.org/) that lets you run blocks asynchronously

## Configuration
The program is configured through a JSON file, which by default is searched at `$HOME/.config/plstatus/config.json`

The JSON file structure includes

| Key           | Description                                  |Optional|Default|
|---------------|----------------------------------------------|--------|-------|
| `components`  | An array of component configurations         | No     | N/A   |
| `period`      | The default update period for the status bar | Yes    | 1000  |
| `first_wait`  | An initial waiting period in milliseconds $^1$  | Yes    | 5     |

Components configuration
----------
| Key           | Description                                    |Optional|Default|
|---------------|------------------------------------------------|--------|-------|
| `binary`      | The binary to execute.                         | No     | N/A   |
| `arguments`   | Array of arguments to pass to the binary       | Yes    | []    |
| `period`      | The update period in milliseconds              | Yes    | 1000  |
| `separator`   | A separator to use between component outputs   | Yes    | \|    |


Refer to `config.def.json` for an example configuration

$^1$ The status bar period refers to the duration waited between each status bar print, independent of individual component status periods

## Installation

Clone the repository, edit the configuration file and run
```
# make install
```

The default install path is `/usr/local/bin` and can be changed via `INSTALL_PATH` environment variable

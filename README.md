# **plstatus**

plstatus (parallel status) is a minimal, JSON configurable status bar for [dwm](https://dwm.suckless.org/) that lets you run blocks asynchronously

## Configuration
The program is configured through a JSON file, which by default is searched at `$HOME/.config/plstatus/config.json`

The JSON configuration file structure includes

| Key           | Description                                       |Optional|Default|
|---------------|---------------------------------------------------|--------|-------|
| `components`  | An array of component configurations              | No     | N/A   |
| `period`      | The default update period for the status bar $^1$ | Yes    | 1000  |
| `first_wait`  | An initial waiting period in milliseconds         | Yes    | 5     |

$^1$ The status bar period refers to the duration waited between each status bar print, independent of individual component status periods

Component configuration
----------
| Key           | Description                                           |Optional|Default         |
|---------------|-------------------------------------------------------|--------|----------------|
| `binary`      | The binary to execute.                                | No     | N/A            |
| `arguments`   | Array of arguments to pass to the binary              | Yes    | []             |
| `period`      | The update period in milliseconds                     | Yes    | $\infty$       |
| `separator`   | A separator to use between component outputs          | Yes    | \|             |
| `label`       | A string to be printed before the component output    | Yes    |                |

Refer to `config.def.json` for an example configuration

## Installation

Clone the repository, edit the configuration file and run
```
$ make all && sudo make install
```

The default install path is `/usr/local/bin` and can be changed via the `INSTALL_PATH` environment variable

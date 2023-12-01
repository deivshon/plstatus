# **plstatus**

plstatus (parallel status) is a minimal, JSON configurable status bar for [dwm](https://dwm.suckless.org/) that lets you run blocks asynchronously

## Configuration

The program is configured through a JSON file, which by default is searched at `$HOME/.config/plstatus/config.json`

All time durations here are expressed - as they must be in the configuration file - in **milliseconds**

The JSON configuration file structure includes

| Key          | Description                                            | Optional | Default                        |
| ------------ | ------------------------------------------------------ | -------- | ------------------------------ |
| `components` | An array of component configurations                   | No       | N/A                            |
| `period`     | The update period for the status bar $^1$              | Yes      | Every time a component updates |
| `firstWait`  | An initial wait before the first status bar print $^2$ | Yes      | 5                              |

$^1$ The status bar period refers to the duration waited between each status bar print, independent of individual component status periods

$^2$ This can be useful because during this period the components will have time to get their first results, and the first bar print will not be empty

## Component configuration

| Key         | Description                                        | Optional | Default      |
| ----------- | -------------------------------------------------- | -------- | ------------ |
| `binary`    | The binary to execute.                             | No       | N/A          |
| `arguments` | Array of arguments to pass to the binary           | Yes      | No arguments |
| `period`    | The update period                                  | Yes      | $\infty$     |
| `separator` | A separator to use after component output          | Yes      | `\|`         |
| `label`     | A string to be printed before the component output | Yes      |              |

Refer to `config.def.json` for an example configuration

## CLI options

| Short | Long       | Description                           | Optional | Default                              |
| ----- | ---------- | ------------------------------------- | -------- | ------------------------------------ |
| `-c`  | `--config` | Path to the configuration file        | Yes      | `$HOME/.config/plstatus/config.json` |
| `-s`  | `--stdout` | Flag to print status output on stdout | Yes      | `false`                              |
| `-d`  | `--debug`  | Flag to print debug output            | Yes      | `false`                              |

## Installation

Clone the repository, edit the configuration file and run

```
$ make all && sudo make install
```

The default install path is `/usr/local/bin` and can be changed via the `INSTALL_PATH` environment variable

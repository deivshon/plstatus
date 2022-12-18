# **plstatus**

plstatus (parallel status) is a minimal and compact (144 significant LOC) status bar for [dwm](https://dwm.suckless.org/) that lets you run blocks asynchronously. JSON configurable, with `config.h` created at compile time

## Shell-less commands invocation 

All dwm status bar programs I encountered use methods of running commands that carry shells with them (such as using `popen`, as in the case of the [*run_command*](https://git.suckless.org/slstatus/file/components/run_command.c.html) module in the original [slstatus](https://tools.suckless.org/slstatus/))

plstatus runs user defined commands asynchronously without invoking a shell

## Configuration
The configuration is done using a JSON file, `config.json`, which is read at compile time by a Python script that creates an equivalent `config.h` file

`config.json` is created after running `make` for the first time. Alternatively, you can create it manually

The accepted `config.json` fields are the following:
+ `max-status-length` - the maximum length in bytes that the status can be
+ `status-period` - the period of the status bar update in milliseconds (indipendent of the components' periods)
+ `separator` - string describing the separator one can use in the `components` field
+ `components` - a list describing the blocks. Each item can be the string "separator" or an object describing a block, which has the following fields:
    + `command` - the name or path of the program to invoke<sup>1</sup>
    + `period` - the period of the component update in milliseconds (independent of the status bar period). If this value is set to zero or less, the component will not be updated
    + `args` (optional) - the arguments to pass the `command`<sup>1</sup>. If only one argument is needed, it can be a string, otherwise, a list of strings is needed

An example configuration file is worth a thousand words: refer to `config.def.json` if something is not clear

<sup>1</sup> When ran through the `config.h` creation script, tilde expansion is performed on commands and arguments, so "~" can be used to describe paths

## How to install

Clone the repository, edit the configuration file and run
```
make clean all && sudo make install
```

The install path is `/usr/local/bin`. If needed, it can be changed in the Makefile

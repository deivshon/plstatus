package main

import (
	"flag"
	"fmt"
	"main/src/pkg/component"
	"main/src/pkg/config"
	"main/src/pkg/utils"
	"main/src/pkg/xorg"
	"os"
	"os/signal"
	"strings"
	"syscall"
	"time"
)

var noSeparator string = ""

func printStatusBar(status string, stdout bool) {
	if stdout {
		utils.Debug("printing status on stdout")
		fmt.Printf("%v\n", status)
	} else {
		utils.Debug("printing status on bar")
		err := xorg.XSetRoot(status)
		if err != nil {
			utils.Warning(fmt.Sprintf("could not run xsetroot: %v", err))
		}
	}
}

func computeStatus(components []component.Component) string {
	status := ""
	for i, c := range components {
		if c.LastStatus == "" {
			continue
		}

		separator := *c.Separator
		if i == len(components)-1 {
			separator = noSeparator
		}

		status = fmt.Sprintf("%v%v%v", status, c.LastStatus, separator)
	}

	return status
}

func main() {
	var debug = flag.Bool("d", false, "Debug output")
	var stdout = flag.Bool("s", false, "Print status on stdout")
	var configPath = flag.String("c", "", "Path to the configuration file to be used")
	flag.Parse()

	signalsChan := make(chan os.Signal, 1)
	signal.Notify(signalsChan, syscall.SIGINT, syscall.SIGTERM)

	if *debug {
		utils.SetDebugMode()
	}

	if *configPath == "" {
		home := os.Getenv("HOME")
		if home == "" {
			utils.Failure("$HOME not set: could not establish default configuration file path and none was passed")
		}

		*configPath = fmt.Sprintf("%v/.config/plstatus/config.json", home)
	}

	config, err := config.Parse(*configPath)
	if err != nil {
		utils.Failure(fmt.Sprintf("%v", err))
	}

	printOnAllUpdates := config.Period == nil

	outputChan := make(chan component.ComponentOutput)
	for i, c := range config.Components {
		go c.Run(i, outputChan)
	}

	var tickerChannel <-chan time.Time
	if !printOnAllUpdates {
		periodTicker := time.NewTicker(time.Duration(*config.Period) * time.Millisecond)
		tickerChannel = periodTicker.C
	}

	firstWaitChannel := time.After(time.Duration(*config.FirstWait) * time.Millisecond)
	for {
		select {
		case output := <-outputChan:
			if output.Id >= len(config.Components) {
				utils.Warning(fmt.Sprintf("returned output has unknown id: %v", output.Id))
				break
			}
			utils.Debug(fmt.Sprintf("received output from component no. %v (%v)", output.Id, *config.Components[output.Id].Binary))

			config.Components[output.Id].LastStatus = strings.TrimSpace(output.Output)

			if !printOnAllUpdates {
				break
			}

			printStatusBar(computeStatus(config.Components), *stdout)
		case s := <-signalsChan:
			utils.Debug(fmt.Sprintf("received signal: %v", s))

			if !*stdout {
				err = xorg.XSetRoot(xorg.DefaultRoot)
				if err != nil {
					utils.Warning(fmt.Sprintf("could not run xsetroot on shutdown: %v", err))
				}
			}

			os.Exit(0)
		case <-tickerChannel:
			utils.Debug("status period ticker")
			printStatusBar(computeStatus(config.Components), *stdout)
		case <-firstWaitChannel:
			utils.Debug("first wait triggered")
			printStatusBar(computeStatus(config.Components), *stdout)
		}
	}
}

package main

import (
	"fmt"
	"main/src/pkg/component"
	"main/src/pkg/config"
	"main/src/pkg/utils"
	"main/src/pkg/xorg"
	"os"
	"os/signal"
	"strings"
	"syscall"
)

var noSeparator string = ""

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
	config, err := config.Parse("./config.def.json")
	if err != nil {
		utils.Failure(fmt.Sprintf("%v", err))
	}

	signalsChan := make(chan os.Signal, 1)
	signal.Notify(signalsChan, syscall.SIGINT, syscall.SIGTERM)

	outputChan := make(chan component.ComponentOutput)
	for i, c := range config.Components {
		go c.Run(i, outputChan)
	}

	for {
		select {
		case output := <-outputChan:
			if output.Id >= len(config.Components) {
				utils.Warning(fmt.Sprintf("returned output has unknown id: %v", output.Id))
			}

			config.Components[output.Id].LastStatus = strings.TrimSpace(output.Output)
			err = xorg.XSetRoot(computeStatus(config.Components))
			if err != nil {
				utils.Warning(fmt.Sprintf("could not run xsetroot: %v", err))
			}
		case <-signalsChan:
			err = xorg.XSetRoot(xorg.DefaultRoot)
			if err != nil {
				utils.Warning(fmt.Sprintf("could not run xsetroot on shutdown: %v", err))
			}
			os.Exit(0)
		}
	}
}

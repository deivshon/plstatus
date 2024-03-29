package component

import (
	"fmt"
	"main/src/pkg/utils"
	"os/exec"
	"time"
)

func (c Component) Run(id int, out chan<- ComponentOutput) {
	firstRun := true
	componentIdentifier := fmt.Sprintf("no. %v (%v)", id, *c.Binary)
	for {
		if !firstRun {
			time.Sleep(time.Duration(*c.Period) * time.Millisecond)
		}
		firstRun = false

		var output string
		command := exec.Command(*c.Binary, c.Arguments...)
		stdout, err := command.Output()
		exitCode := command.ProcessState.ExitCode()
		if err != nil {
			utils.Warning(fmt.Sprintf("component %v has errored out: %v", componentIdentifier, err))
			output = ""
		} else if exitCode != 0 {
			utils.Warning(fmt.Sprintf("component %v exited with code: %v", componentIdentifier, exitCode))
			output = ""
		} else {
			output = fmt.Sprintf("%v%v", c.Label, string(stdout))
		}

		out <- ComponentOutput{
			Output: output,
			Id:     id,
		}

		if c.Period == nil {
			break
		}
	}

	utils.Debug(fmt.Sprintf("exiting component %v run goroutine", componentIdentifier))
}

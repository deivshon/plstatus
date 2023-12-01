package xorg

import (
	"fmt"
	"os/exec"
)

const DefaultRoot string = ""

func XSetRoot(name string) error {
	command := exec.Command("xsetroot", "-name", name)
	err := command.Run()
	exitCode := command.ProcessState.ExitCode()
	if err != nil {
		return err
	} else if exitCode != 0 {
		return fmt.Errorf("xsetroot command exited with status: %v", exitCode)
	}

	return nil
}

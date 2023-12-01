package config

import (
	"encoding/json"
	"fmt"
	"os"
)

const defaultFirstWait int32 = 10
const defaultComponentPeriod int32 = 1000
const defaultComponentSeparator string = "|"
const defaultLastStatus string = "N/A"

type InvalidConfigError struct {
	Message string
}

func (e InvalidConfigError) Error() string {
	return fmt.Sprintf("invalid config detected: %v", e.Message)
}

func newInvalidConfigError(message string) InvalidConfigError {
	return InvalidConfigError{
		Message: message,
	}
}

func Parse(filepath string) (Config, error) {
	data, err := os.ReadFile(filepath)
	if err != nil {
		return Config{}, err
	}

	var config Config
	err = json.Unmarshal(data, &config)
	if err != nil {
		return Config{}, err
	}

	if config.Period == nil {
		return Config{}, newInvalidConfigError("a status bar period must be specified")
	}

	if config.FirstWait == nil {
		firstWait := defaultFirstWait
		config.FirstWait = &firstWait
	}

	for i := 0; i < len(config.Components); i++ {
		component := config.Components[i]

		if component.Binary == nil {
			return Config{}, newInvalidConfigError("could not find binary field for component")
		}
		if component.Arguments == nil {
			config.Components[i].Arguments = make([]string, 0)
		}
		if component.Period == nil {
			period := defaultComponentPeriod
			config.Components[i].Period = &period
		}
		if component.Separator == nil {
			separator := defaultComponentSeparator
			config.Components[i].Separator = &separator
		}

		component.LastStatus = defaultLastStatus
	}

	return config, nil
}

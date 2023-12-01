package config

import "main/src/pkg/component"

type Config struct {
	Period     *int32                `json:"period"`
	Components []component.Component `json:"components"`
	FirstWait  *int32                `json:"firstWait"`
}

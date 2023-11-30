package config

type Config struct {
	Period     *int32         `json:"period"`
	Components []RawComponent `json:"components"`
	FirstWait  *int32         `json:"first_wait"`
}

type RawComponent struct {
	Binary    *string  `json:"binary"`
	Arguments []string `json:"arguments"`
	Period    *int32   `json:"period"`
	Separator *string  `json:"separator"`
	Label     string   `json:"label"`
}

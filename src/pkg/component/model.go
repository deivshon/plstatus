package component

type Component struct {
	Binary     *string  `json:"binary"`
	Arguments  []string `json:"arguments"`
	Period     *int32   `json:"period"`
	Separator  *string  `json:"separator"`
	Label      string   `json:"label"`
	LastStatus string
}

type ComponentOutput struct {
	Output string
	Id     int
}

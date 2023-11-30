package main

import (
	"fmt"
	"main/src/pkg/config"
	"main/src/pkg/utils"
)

func main() {
	config, err := config.Parse("./config.def.json")
	if err != nil {
		utils.Failure(fmt.Sprintf("%+v", err))
	}

	fmt.Println(*config.Period)
	fmt.Println(*config.FirstWait)
	for _, c := range config.Components {
		fmt.Println(*c.Binary)
		fmt.Println(c.Arguments)
		fmt.Println(*c.Period)
	}
}

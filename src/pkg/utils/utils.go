package utils

import (
	"log"
	"os"
)

var debugMode = false
var debugLogger = log.New(os.Stdout, "", log.LstdFlags)

func Failure(message string) {
	log.Fatalf("fatal failure: %v\n", message)
}

func Warning(message string) {
	log.Printf("warning: %v\n", message)
}

func Debug(message string) {
	if debugMode {
		debugLogger.Printf("debug: %v\n", message)
	}
}

func SetDebugMode() {
	debugMode = true
}

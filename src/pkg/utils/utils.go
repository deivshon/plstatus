package utils

import (
	"log"
	"os"
)

var debugMode = false
var debugLogger = log.New(os.Stdout, "", log.Ldate|log.Lmicroseconds)
var stderrLogger = log.New(os.Stderr, "", log.Ldate|log.Lmicroseconds)

func Failure(message string) {
	stderrLogger.Fatalf("failure: %v\n", message)
}

func Warning(message string) {
	stderrLogger.Printf("warning: %v\n", message)
}

func Debug(message string) {
	if debugMode {
		debugLogger.Printf("debug: %v\n", message)
	}
}

func SetDebugMode() {
	debugMode = true
}

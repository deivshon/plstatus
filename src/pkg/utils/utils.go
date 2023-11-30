package utils

import "log"

func Failure(message string) {
	log.Fatalf("fatal failure: %v", message)
}

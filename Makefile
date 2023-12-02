INSTALL_PATH ?= /usr/local/bin
PLSTATUS_BINARY = ./out/release/plstatus

debug:
	mkdir -p ./out/debug
	go mod tidy && make lint && go build -o ./out/debug/plstatus ./src/cmd/main.go

release:
	mkdir -p ./out/release
	go mod tidy && go build -ldflags "-w -s" -o $(PLSTATUS_BINARY) ./src/cmd/main.go

lint:
	golangci-lint run

install: $(PLSTATUS_BINARY)
	mkdir -p $(INSTALL_PATH)
	cp -f $(PLSTATUS_BINARY) $(INSTALL_PATH)
	chmod 755 $(INSTALL_PATH)/plstatus

uninstall:
	rm -f $(INSTALL_PATH)/plstatus

clean:
	rm -rf ./out

.PHONY: all install uninstall clean

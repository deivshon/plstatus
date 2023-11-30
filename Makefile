INSTALL_PATH ?= /usr/local/bin
PLSTATUS_BINARY = ./out/plstatus

all:
	mkdir -p ./out
	go build -o ./out/plstatus ./src/cmd/main.go

install: $(PLSTATUS_BINARY)
	mkdir -p $(INSTALL_PATH)
	cp -f $(PLSTATUS_BINARY) $(INSTALL_PATH)
	chmod 755 $(INSTALL_PATH)/plstatus

uninstall:
	rm -f $(INSTALL_PATH)/plstatus

clean:
	rm -rf ./out

.PHONY: all install uninstall clean

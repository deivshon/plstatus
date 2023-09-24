INSTALL_PATH ?= /usr/local/bin
PLSTATUS_BINARY = ./target/release/plstatus

all:
	cargo build --release

install: $(PLSTATUS_BINARY)
	mkdir -p $(INSTALL_PATH)
	cp -f $(PLSTATUS_BINARY) $(INSTALL_PATH)
	chmod 755 $(INSTALL_PATH)/plstatus

uninstall:
	rm -f $(INSTALL_PATH)/plstatus

clean:
	cargo clean

.PHONY: all install uninstall clean

INSTALL_PATH = /usr/local/bin

all:
	cargo build --release

install: all
	mkdir -p $(INSTALL_PATH)
	cp -f ./target/release/plstatus $(INSTALL_PATH)
	chmod 755 $(INSTALL_PATH)/plstatus

uninstall:
	rm -f $(INSTALL_PATH)/plstatus

clean:
	cargo clean

.PHONY: all install uninstall clean

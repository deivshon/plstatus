CC = gcc

CFLAGS = -O2
LDFLAGS = -lX11 -lpthread

INSTALL_PATH = /usr/local/bin

all: config.h plstatus

plstatus: plstatus.o
	$(CC) -o $@ $(LDFLAGS) $<

plstatus.o: plstatus.c plstatus.h config.h
	$(CC) -c -o $@ $(CFLAGS) $<

config.h: config.json make-config.py
	python3 make-config.py

config.json: config.def.json
	cp config.def.json config.json

install: plstatus
	mkdir -p $(INSTALL_PATH)
	cp plstatus $(INSTALL_PATH)
	chmod 755 $(INSTALL_PATH)/plstatus

uninstall:
	rm -f $(INSTALL_PATH)/plstatus

clean:
	rm -f plstatus config.h *.o

.PHONY: all install uninstall clean

CC = gcc

CFLAGS = -O2 -Wall -Wextra
LDFLAGS = -lX11 -lpthread

SRC = src
HS = src
CFG = .

INSTALL_PATH = /usr/local/bin

all: $(HS)/config.h plstatus

plstatus: $(SRC)/plstatus.o
	$(CC) -o $@ $(LDFLAGS) $<

$(SRC)/plstatus.o: $(SRC)/plstatus.c $(HS)/plstatus.h $(HS)/config.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(HS)/config.h: config.json $(CFG)/make-config.py
	python3 $(CFG)/make-config.py

config.json: config.def.json
	cp config.def.json config.json

install: plstatus
	mkdir -p $(INSTALL_PATH)
	cp -f plstatus $(INSTALL_PATH)
	chmod 755 $(INSTALL_PATH)/plstatus

uninstall:
	rm -f $(INSTALL_PATH)/plstatus

clean:
	rm -f plstatus $(SRC)/*.o $(HS)/config.h

.PHONY: all install uninstall clean

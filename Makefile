CC = gcc

CFLAGS = -O2
LDFLAGS = -lX11 -lpthread

INSTALL_PATH = /usr/local/bin

all: plstatus

plstatus: plstatus.o
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<

%.o: %.c %.h
	$(CC) -c -o $@ $(CFLAGS) $<

install: plstatus
	mkdir -p $(INSTALL_PATH)
	cp plstatus $(INSTALL_PATH)
	chmod 755 $(INSTALL_PATH)/plstatus

uninstall:
	rm -f $(INSTALL_PATH)/plstatus

clean:
	rm plstatus *.o

.PHONY: all install uninstall clean

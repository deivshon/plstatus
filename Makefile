plstatus: plstatus.c config.h
	gcc -o $@ -lX11 -lpthread $<

clean:
	rm plstatus

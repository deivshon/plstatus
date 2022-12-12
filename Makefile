plstatus: plstatus.c plstatus.h config.h
	gcc -o $@ -lX11 -lpthread $<

clean:
	rm plstatus

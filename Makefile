plstatus: plstatus.c
	gcc -o $@ -lX11 -lpthread $<
	
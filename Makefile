CFLAGS+=-O2 -Wall

main: transpos

transpos: transpos.c
	$(CC) -o $@ $^ $(CFLAGS)


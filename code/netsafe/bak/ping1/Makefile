CC=gcc
CFLAGS=-Wall -g -DDEBUG
LDFLAGS=
LIBS=

all:  ping

ping: ping.c 
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

clean:
	rm -rf *.o

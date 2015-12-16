CC=gcc
CFLAGS=-Wall -g -DDEBUG
LDFLAGS=
LIBS=

all:  ping pingsend pingrecv

ping: ping.c 
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)
pingsend: pingsend.c 
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)
pingrecv: pingrecv.c 
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

clean:
	rm -rf *.o
	rm ping pingsend pingrecv -f

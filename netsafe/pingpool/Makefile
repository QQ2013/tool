CC=gcc
CFLAGS=-Wall -g -DDEBUG
LDFLAGS=
LIBS=

all: ping_ip.o ping

ping_ip.o: ping_ip.c
	$(CC)  -o $@ $(LDFLAGS) -c $^ $(LIBS)

ping: ping.c ping_ip.o
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

clean:
	rm -rf *.o



all: udp.o dns.o

dns.o: dns.c dns.h
	$(CC) $(CFLAGS) -o dns.o -c dns.c

udp.o: udp.c udp.h
	$(CC) $(CFLAGS) -o udp.o -c udp.c

clean:
	rm -rf *.o

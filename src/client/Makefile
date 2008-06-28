

all: client.o configFile.o font.o game.o image.o interface.o keytable.o language.o layer.o panel.o screen.o term.o

client.o: client.c client.h
	$(CC) $(CFLAGS) -o client.o -c client.c

configFile.o: configFile.c configFile.h
	$(CC) $(CFLAGS) -o configFile.o -c configFile.c

font.o: font.c font.h
	$(CC) $(CFLAGS) -o font.o -c font.c

game.o: game.c game.h
	$(CC) $(CFLAGS) -o game.o -c game.c

image.o: image.c image.h
	$(CC) $(CFLAGS) -o image.o -c image.c

interface.o: interface.c interface.h
	$(CC) $(CFLAGS) -o interface.o -c interface.c

layer.o: layer.c layer.h
	$(CC) $(CFLAGS) -o layer.o -c layer.c

panel.o: panel.c panel.h
	$(CC) $(CFLAGS) -o panel.o -c panel.c

screen.o: screen.c screen.h
	$(CC) $(CFLAGS) -o screen.o -c screen.c

term.o: term.c term.h
	$(CC) $(CFLAGS) -o term.o -c term.c

clean:
	rm -rf *.o

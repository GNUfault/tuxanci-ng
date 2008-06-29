
all: modWall.so modPipe.so modTeleport.so modAI.so

list.o: list.c list.h
	$(CC) $(CFLAGS) -fPIC -o list.o -c list.c

space.o: space.c space.h
	$(CC) $(CFLAGS) -fPIC -o space.o -c space.c

modWall.so: list.o space.o
	gcc $(CFLAGS) -fPIC -c modWall.c -o modWall.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modWall.so modWall.o list.o space.o

modPipe.so: list.o space.o
	gcc $(CFLAGS) -fPIC -c modPipe.c -o modPipe.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modPipe.so modPipe.o list.o space.o

modTeleport.so: list.o space.o
	gcc $(CFLAGS) -fPIC -c modTeleport.c -o modTeleport.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modTeleport.so modTeleport.o list.o space.o

modAI.so: list.o space.o
	gcc $(CFLAGS) -fPIC -c modAI.c -o modAI.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modAI.so modAI.o list.o space.o

clean:
	rm -rf *.o 
	rm -rf *.so


all: modWall.so modPipe.so modTeleport.so modAI.so

list.o: ../base/list.c ../base/list.h
	$(CC) $(CFLAGS) -fPIC -o list.o -c ../base/list.c

space.o: ../base/space.c ../base/space.h
	$(CC) $(CFLAGS) -fPIC -o space.o -c ../base/space.c

index.o: ../base/index.c ../base/index.h
	$(CC) $(CFLAGS) -fPIC -o index.o -c ../base/index.c

modWall.so: list.o space.o index.o
	gcc $(CFLAGS) -fPIC -c modWall.c -o modWall.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modWall.so modWall.o list.o space.o index.o

modPipe.so: list.o space.o index.o
	gcc $(CFLAGS) -fPIC -c modPipe.c -o modPipe.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modPipe.so modPipe.o list.o space.o index.o

modTeleport.so: list.o space.o index.o
	gcc $(CFLAGS) -fPIC -c modTeleport.c -o modTeleport.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modTeleport.so modTeleport.o list.o space.o index.o

modAI.so: list.o space.o index.o
	gcc $(CFLAGS) -fPIC -c modAI.c -o modAI.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modAI.so modAI.o list.o space.o index.o

clean:
	rm -rf *.o 
	rm -rf *.so

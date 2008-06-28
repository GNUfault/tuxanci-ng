
all: modWall.so modPipe.so modTeleport.so modAI.so

modWall.so:
	gcc $(CFLAGS) -fPIC -c modWall.c -o modWall.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modWall.so modWall.o ../base/list.o ../base/space.o

modPipe.so:
	gcc $(CFLAGS) -fPIC -c modPipe.c -o modPipe.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modPipe.so modPipe.o ../base/list.o ../base/space.o

modTeleport.so:
	gcc $(CFLAGS) -fPIC -c modTeleport.c -o modTeleport.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modTeleport.so modTeleport.o ../base/list.o ../base/space.o

modAI.so:
	gcc $(CFLAGS) -fPIC -c modAI.c -o modAI.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modAI.so modAI.o ../base/list.o ../base/space.o

clean:
	rm -rf *.o 
	rm -rf *.so

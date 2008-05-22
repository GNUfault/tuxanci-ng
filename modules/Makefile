
CFLAGS = -g -O0  -I../include -Wall

modTeleport:
	gcc $(CFLAGS) -fPIC -I../include `sdl-config --cflags` -c list.c -o list.o
	gcc $(CFLAGS) -fPIC -I../include `sdl-config --cflags` -c modTeleport.c -o modTeleport.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modTeleport.so modTeleport.o list.o

modPipe:
	gcc $(CFLAGS) -fPIC -I../include `sdl-config --cflags` -c list.c -o list.o
	gcc $(CFLAGS) -fPIC -I../include `sdl-config --cflags` -c modPipe.c -o modPipe.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modPipe.so modPipe.o list.o

modWall:
	gcc $(CFLAGS) -fPIC -I../include `sdl-config --cflags` -c list.c -o list.o
	gcc $(CFLAGS) -fPIC -I../include `sdl-config --cflags` -c modWall.c -o modWall.o
	gcc $(CFLAGS) -I../include -shared -fPIC -o modWall.so modWall.o list.o

clean:
	rm -rf *.o 
	rm -rf *.so

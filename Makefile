CFLAGS= -m32 -O2 -Wall -lpthread
CPP= g++

preklad: main.cpp
	$(CPP) $(CFLAGS) -otuxanci-master main.cpp sockets.cpp resolve.cpp

clean:
	rm -f tuxanci-master

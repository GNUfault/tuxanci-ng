

all: audio.o music.o sound.o

audio.o: audio.c audio.h
	$(CC) $(CFLAGS) -o audio.o -c audio.c

music.o: music.c music.h
	$(CC) $(CFLAGS) -o music.o -c music.c

sound.o: sound.c sound.h
	$(CC) $(CFLAGS) -o sound.o -c sound.c



clean:
	rm -rf *.o

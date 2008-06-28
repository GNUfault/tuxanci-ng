

all: heightScore.o publicServer.o serverConfigFile.o

heightScore.o: heightScore.c heightScore.h
	$(CC) $(CFLAGS) -o heightScore.o -c heightScore.c

publicServer.o: publicServer.c publicServer.h
	$(CC) $(CFLAGS) -o publicServer.o -c publicServer.c

serverConfigFile.o: serverConfigFile.c serverConfigFile.h
	$(CC) $(CFLAGS) -o serverConfigFile.o -c serverConfigFile.c

clean:
	rm -rf *.o

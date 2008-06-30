
all: screen_analyze.o screen_browser.o screen_credits.o screen_gameType.o screen_choiceArena.o screen_mainMenu.o screen_setting.o screen_table.o screen_world.o

screen_analyze.o: screen_analyze.c screen_analyze.h
	$(CC) $(CFLAGS) -o screen_analyze.o -c screen_analyze.c

screen_browser.o: screen_browser.c screen_browser.h
	$(CC) $(CFLAGS) -o screen_browser.o -c screen_browser.c

screen_credits.o: screen_credits.c screen_credits.h
	$(CC) $(CFLAGS) -o screen_credits.o -c screen_credits.c

screen_gameType.o: screen_gameType.c screen_gameType.h
	$(CC) $(CFLAGS) -o screen_gameType.o -c screen_gameType.c

screen_mainMenu.o: screen_mainMenu.c screen_mainMenu.h
	$(CC) $(CFLAGS) -o screen_mainMenu.o -c screen_mainMenu.c

screen_setting.o: screen_setting.c screen_setting.h
	$(CC) $(CFLAGS) -o screen_setting.o -c screen_setting.c

screen_table.o: screen_table.c screen_table.h
	$(CC) $(CFLAGS) -o screen_table.o -c screen_table.c

screen_world.o: screen_world.c screen_world.h
	$(CC) $(CFLAGS) -o screen_world.o -c screen_world.c

clean:
	rm -rf *.o




#CC = /usr/local/gcc/bin/gcc
CC = gcc

CFLAGS = -g -O0 -DSUPPORT_NET_SDL_UDP -DDISTDIR=\"$(DISTDIR)\" -I../include -Wall `sdl-config --cflags`
#CFLAGS = -O2 -DDISTDIR=\"$(DISTDIR)\" `sdl-config --cflags` -I../include -Wall

LIBS = `sdl-config --libs` -lSDL_image -lSDL_ttf -lSDL_mixer -lSDL_net

FILES = interface.o font.o list.o image.o layer.o director.o configFile.o tux.o main.o \
	screen.o screen_world.o wall.o shot.o myTimer.o panel.o net_multiplayer.o  \
	buffer.o dynamicInt.o arena.o arenaFile.o textFile.o sound.o audio.o music.o gun.o \
	item.o widget_label.o screen_mainMenu.o widget_button.o screen_analyze.o widget_textfield.o \
	widget_check.o widget_image.o screen_setting.o screen_gameType.o screen_choiceArena.o \
	widget_buttonimage.o screen_credits.o teleport.o pipe.o homeDirector.o screen_table.o \
	sdl_udp.o proto.o server.o client.o
 
tuxanci-ng: $(FILES)
	$(CC) $(CFLAGS) $(LIBS) -o tuxanci-ng $(FILES)

main.o: main.c 
	$(CC) $(CFLAGS) -o main.o -c main.c

interface.o: interface.c ../include/interface.h
	$(CC) $(CFLAGS) -o interface.o -c interface.c

font.o: font.c ../include/font.h
	$(CC) $(CFLAGS) -o font.o -c font.c

list.o: list.c ../include/list.h
	$(CC) $(CFLAGS) -o list.o -c list.c

image.o: image.c ../include/image.h
	$(CC) $(CFLAGS) -o image.o -c image.c

layer.o: layer.c ../include/layer.h
	$(CC) $(CFLAGS) -o layer.o -c layer.c

director.o: director.c ../include/director.h
	$(CC) $(CFLAGS) -o director.o -c director.c

configFile.o: configFile.c ../include/configFile.h
	$(CC) $(CFLAGS) -o configFile.o -c configFile.c

tux.o: tux.c ../include/tux.h
	$(CC) $(CFLAGS) -o tux.o -c tux.c

screen.o: screen.c ../include/screen.h
	$(CC) $(CFLAGS) -o screen.o -c screen.c

screen_world.o: screen_world.c ../include/screen_world.h
	$(CC) $(CFLAGS) -o screen_world.o -c screen_world.c

wall.o: wall.c ../include/wall.h
	$(CC) $(CFLAGS) -o wall.o -c wall.c

shot.o: shot.c ../include/shot.h
	$(CC) $(CFLAGS) -o shot.o -c shot.c

myTimer.o: myTimer.c ../include/myTimer.h
	$(CC) $(CFLAGS) -o myTimer.o -c myTimer.c

panel.o: panel.c ../include/panel.h
	$(CC) $(CFLAGS) -o panel.o -c panel.c

buffer.o: buffer.c ../include/buffer.h
	$(CC) $(CFLAGS) -o buffer.o -c buffer.c

tcp.o: tcp.c ../include/tcp.h
	$(CC) $(CFLAGS) -o tcp.o -c tcp.c

udp.o: udp.c ../include/udp.h
	$(CC) $(CFLAGS) -o udp.o -c udp.c

sdl_udp.o: sdl_udp.c ../include/sdl_udp.h
	$(CC) $(CFLAGS) -o sdl_udp.o -c sdl_udp.c

proto.o: proto.c ../include/proto.h
	$(CC) $(CFLAGS) -o proto.o -c proto.c

server.o: server.c ../include/server.h
	$(CC) $(CFLAGS) -o server.o -c server.c

client.o: client.c ../include/client.h
	$(CC) $(CFLAGS) -o client.o -c client.c
network.o: network.c ../include/network.h
	$(CC) $(CFLAGS) -o network.o -c network.c

net_multiplayer.o: net_multiplayer.c ../include/net_multiplayer.h
	$(CC) $(CFLAGS) -o net_multiplayer.o -c net_multiplayer.c

dynamicInt.o: dynamicInt.c ../include/dynamicInt.h
	$(CC) $(CFLAGS) -o dynamicInt.o -c dynamicInt.c

arena.o: arena.c ../include/arena.h
	$(CC) $(CFLAGS) -o arena.o -c arena.c

textFile.o: textFile.c ../include/textFile.h
	$(CC) $(CFLAGS) -o textFile.o -c textFile.c

arenaFile.o: arenaFile.c ../include/arenaFile.h
	$(CC) $(CFLAGS) -o arenaFile.o -c arenaFile.c

audio.o: audio.c ../include/audio.h
	$(CC) $(CFLAGS) -o audio.o -c audio.c

sound.o: sound.c ../include/sound.h
	$(CC) $(CFLAGS) -o sound.o -c sound.c

music.o: music.c ../include/music.h
	$(CC) $(CFLAGS) -o music.o -c music.c

gun.o: gun.c ../include/gun.h
	$(CC) $(CFLAGS) -o gun.o -c gun.c

item.o: item.c ../include/item.h
	$(CC) $(CFLAGS) -o item.o -c item.c

screen_mainMenu.o: screen_mainMenu.c ../include/screen_mainMenu.h
	$(CC) $(CFLAGS) -o screen_mainMenu.o -c screen_mainMenu.c

screen_analyze.o: screen_analyze.c ../include/screen_analyze.h
	$(CC) $(CFLAGS) -o screen_analyze.o -c screen_analyze.c

screen_setting.o: screen_setting.c ../include/screen_setting.h
	$(CC) $(CFLAGS) -o screen_setting.o -c screen_setting.c

screen_gameType.o: screen_gameType.c ../include/screen_gameType.h
	$(CC) $(CFLAGS) -o screen_gameType.o -c screen_gameType.c

screen_choiceArena.o: screen_choiceArena.c ../include/screen_choiceArena.h
	$(CC) $(CFLAGS) -o screen_choiceArena.o -c screen_choiceArena.c

screen_credits.o: screen_credits.c ../include/screen_credits.h
	$(CC) $(CFLAGS) -o screen_credits.o -c screen_credits.c

widget_label.o: widget_label.c ../include/widget_label.h
	$(CC) $(CFLAGS) -o widget_label.o -c widget_label.c

widget_button.o: widget_button.c ../include/widget_button.h
	$(CC) $(CFLAGS) -o widget_button.o -c widget_button.c

widget_buttonimage.o: widget_buttonimage.c ../include/widget_buttonimage.h
	$(CC) $(CFLAGS) -o widget_buttonimage.o -c widget_buttonimage.c

widget_textfield.o: widget_textfield.c ../include/widget_textfield.h
	$(CC) $(CFLAGS) -o widget_textfield.o -c widget_textfield.c

widget_check.o: widget_check.c ../include/widget_check.h
	$(CC) $(CFLAGS) -o widget_check.o -c widget_check.c

widget_image.o: widget_image.c ../include/widget_image.h
	$(CC) $(CFLAGS) -o widget_image.o -c widget_image.c

teleport.o: teleport.c ../include/teleport.h
	$(CC) $(CFLAGS) -o teleport.o -c teleport.c

pipe.o: pipe.c ../include/pipe.h
	$(CC) $(CFLAGS) -o pipe.o -c pipe.c

homeDirector.o: homeDirector.c ../include/homeDirector.h
	$(CC) $(CFLAGS) -o homeDirector.o -c homeDirector.c

screen_table.o: screen_table.c ../include/screen_table.h
	$(CC) $(CFLAGS) -o screen_table.o -c screen_table.c

clean:
	rm -rf *.o *.c~ ../include/*.h~ Makefile~ tuxanci-ng

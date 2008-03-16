
#DISTDIR=/usr/local
DISTDIR:=/usr/

all:
	make -C ./src DISTDIR=$(DISTDIR)

clean:
	make -C ./src DISTDIR=$(DISTDIR) clean
	make -C ./src -f Makefile-publicServer clean

run:
	./tuxanci-ng.sh

srun:
	./publicserver.sh

package:
	./deb-build.sh

kill:
	kill -9 `pidof publicserver`

stat:
	cat /proc/`pidof publicserver`/status

tuxstat:
	cat /proc/`pidof tuxanci-ng`/status

line:
	cat src/*.c include*.h | wc -l

core:
	gcore `pidof publicserver`

svn:
	svn ci

svnr:
	svn co http://opensvn.csie.org/tuxanci_ng/

server:
	make -C ./src -f Makefile-publicServer

install:
	cp  ./src/tuxanci-ng $(DISTDIR)bin/
	mkdir $(DISTDIR)share/tuxanci-ng
	mkdir $(DISTDIR)share/tuxanci-ng/arena
	mkdir $(DISTDIR)share/tuxanci-ng/font
	mkdir $(DISTDIR)share/tuxanci-ng/image
	mkdir $(DISTDIR)share/tuxanci-ng/lang
	mkdir $(DISTDIR)share/tuxanci-ng/music
	mkdir $(DISTDIR)share/tuxanci-ng/sound
	mkdir $(DISTDIR)share/tuxanci-ng/data

	cp  -rf ./arena/* $(DISTDIR)share/tuxanci-ng/arena
	cp  -rf ./font/* $(DISTDIR)share/tuxanci-ng/font
	cp  -rf ./image/* $(DISTDIR)share/tuxanci-ng/image
	cp  -rf ./lang/* $(DISTDIR)share/tuxanci-ng/lang
	cp  -rf ./music/* $(DISTDIR)share/tuxanci-ng/music
	cp  -rf ./sound/* $(DISTDIR)share/tuxanci-ng/sound
	cp  -rf ./data/* $(DISTDIR)share/tuxanci-ng/data

uninstall:
	rm -rf $(DISTDIR)bin/tuxanci-ng
	rm -rf $(DISTDIR)share/tuxanci-ng

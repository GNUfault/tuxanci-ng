DESTDIR:=/usr/local

all:
	make -C ./src 

clean:
	make -C ./src clean
	make -C ./src -f Makefile-publicServer clean
	make -C ./modules -f Makefile clean

run:
	./tuxanci-ng.sh

srun:
	./publicserver.sh

package:
	./deb-build.sh

tarball:
	./tarball-build.sh

tarballbin:
	./tarballbin-build.sh

kill:
	kill -9 `pidof publicserver`

stat:
	cat /proc/`pidof publicserver`/status

tuxstat:
	cat /proc/`pidof tuxanci-ng`/status

line:
	cat src/*.c include/*.h | wc -l

core:
	gcore `pidof publicserver`

svn:
	svn ci

svnr:
	svn co http://opensvn.csie.org/tuxanci_ng/

server:
	make -C ./src -f Makefile-publicServer

nosound:
	make -C ./src -f Makefile-nosound

mod:
	make -C ./modules -f Makefile modTeleport
	make -C ./modules -f Makefile modPipe
	make -C ./modules -f Makefile modWall

install:
	mkdir -p $(DESTDIR)/bin
	mkdir -p $(DESTDIR)/share/tuxanci-ng/{arena,data,font,image,lang,music,sound,conf,modules}
	cp  ./src/tuxanci-ng $(DESTDIR)/bin/

	cp  -rf ./arena/* $(DESTDIR)/share/tuxanci-ng/arena/
	cp  -rf ./font/* $(DESTDIR)/share/tuxanci-ng/font/
	cp  -rf ./image/* $(DESTDIR)/share/tuxanci-ng/image/
	cp  -rf ./lang/* $(DESTDIR)/share/tuxanci-ng/lang/
	cp  -rf ./music/* $(DESTDIR)/share/tuxanci-ng/music/
	cp  -rf ./sound/* $(DESTDIR)/share/tuxanci-ng/sound/
	cp  -rf ./data/* $(DESTDIR)/share/tuxanci-ng/data/
	cp  -rf ./conf/* $(DESTDIR)/share/tuxanci-ng/conf/
	cp  -rf ./modules/*.so $(DESTDIR)/share/tuxanci-ng/modules/

uninstall:
	rm -rf $(DESTDIR)/{bin,share}/tuxanci-ng

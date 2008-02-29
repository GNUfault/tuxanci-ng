
#DIST_DIR=/usr/local
DIST_DIR=/usr

all:
	make -C ./src
 
clean:
	make -C ./src clean

run:
	./tuxanci-ng.sh

svn:
	svn ci

svnr:
	svn co http://opensvn.csie.org/tuxanci_ng/

install:
	cp  ./src/tuxanci-ng $(DIST_DIR)/bin/
	mkdir $(DIST_DIR)/share/tuxanci-ng
	mkdir $(DIST_DIR)/share/tuxanci-ng/arena
	mkdir $(DIST_DIR)/share/tuxanci-ng/font
	mkdir $(DIST_DIR)/share/tuxanci-ng/image
	mkdir $(DIST_DIR)/share/tuxanci-ng/lang
	mkdir $(DIST_DIR)/share/tuxanci-ng/music
	mkdir $(DIST_DIR)/share/tuxanci-ng/sound

	cp  -rf ./arena/* $(DIST_DIR)/share/tuxanci-ng/arena
	cp  -rf ./font/* $(DIST_DIR)/share/tuxanci-ng/font
	cp  -rf ./image/* $(DIST_DIR)/share/tuxanci-ng/image
	cp  -rf ./lang/* $(DIST_DIR)/share/tuxanci-ng/lang
	cp  -rf ./music/* $(DIST_DIR)/share/tuxanci-ng/music
	cp  -rf ./sound/* $(DIST_DIR)/share/tuxanci-ng/sound

deb:
	./deb-build.sh

uninstall:
	rm -rf $(DIST_DIR)/bin/tuxanci-ng
	rm -rf $(DIST_DIR)/share/tuxanci-ng

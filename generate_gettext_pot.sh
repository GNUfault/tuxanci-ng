#!/bin/bash
xgettext --default-domain=tuxanci --add-comments=/// -k_ `find ./ -name \*.c | grep -v .git | tr '\n' ' '` -o ./po/tuxanci.pot
cd po/
find ./ -name \*.po -print | xargs -i msgmerge -U {} tuxanci.pot

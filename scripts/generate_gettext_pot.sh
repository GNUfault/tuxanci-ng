#!/bin/bash
PWD=$(pwd)
[[ ${PWD/*\//} = scripts ]] && pushd .. > /dev/null || pushd . > /dev/null
xgettext --no-wrap --width 1 --default-domain=tuxanci --msgid-bugs-address=team@tuxanci.org --add-comments=/// -k_ `find ./ -name \*.c | grep -v .git | sort | uniq | tr '\n' ' '` -o ./po/tuxanci.pot
cd po/
find ./ -name \*.po -print | xargs -i msgmerge --no-wrap --width 1 -U {} tuxanci.pot
rm -rf *~
popd > /dev/null

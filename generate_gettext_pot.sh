#!/bin/bash
xgettext --add-comments=/// -k_ `find ./ -name \*.c |grep -v .svn |tr '\n' ' '` -o ../po/tuxanci.pot

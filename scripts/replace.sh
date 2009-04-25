#!/bin/sh

# '@' - this char is prohibited to use
oldtext="$1"
newtext="$2"

GoThroughDir() {
	for D in $1*; do
		if [ -d $D ] && [ "$D" == "./.git" ]; then
			continue
		elif [ -d $D ] && [ "$D" == "./build" ]; then
			continue
		elif [ -d $D ]; then
			GoThroughDir $D/
		elif [ "$D" == "$0" ]; then
			continue
		else
			sed -i -e "s@$oldtext@$newtext@" $D
		fi
	done
}

GoThroughDir ./

grep -iHr "$oldtext" ./ | grep -v .git/

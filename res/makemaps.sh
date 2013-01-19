#!/bin/bash

#windows fails to symlink
HOSTOS=linux
uname|grep MINGW && HOSTOS=windows

function getmapdata {
	if [ "$HOSTOS" == "windows" ]; then
		cp -r "$1" mapdata
	else
		ln -s "$1" mapdata
	fi
}

mkdir -p ../maps
for map in maps/*; do
	echo " [DFSI] res/$map"
	getmapdata "$map"
	darnit-fsimage mapdata "../maps/`basename "$map"`.ldi"
	rm -rf mapdata
done	

#!/bin/sh
mkdir -p ../maps
for map in maps/*; do
	echo " [DFSI] res/$map"
	ln -s "$map" mapdata
	darnit-fsimage mapdata "../maps/`basename "$map"`.ldi"
	rm mapdata
done	

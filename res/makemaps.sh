#!/bin/sh
for map in maps/*; do
	echo " [DFSI] res/$map"
	ln -s "$map" mapdata
	darnit-fsimage mapdata "`basename "$map"`.ldi"
	rm mapdata
done	

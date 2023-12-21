#!/bin/bash

if [ $# -ne 2 ]
then
	echo "Usage: $0 old new" >&2
	exit
fi

for f in `find -name "*$1*"`
do
	mv $f `echo $f | sed "s/$1/$2/"`
done

for f in `grep -rl $1`
do
	sed -i "s/$1/$2/g" $f
done

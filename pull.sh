#!/bin/bash

d=`find -maxdepth 3 -name .git`
if [ -z "$d" ]
then
	echo no repo
	exit
fi

for d in `readlink -f $d | xargs dirname`
do
	cd $d
	pwd
	git pull
done

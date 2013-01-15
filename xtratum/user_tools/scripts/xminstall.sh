#! /bin/bash

source `dirname $0`/Config.scr

unloader=$USER_TOOLS_PATH/xmloader/unloader.xm

dom_unload()
{
	domslist=`grep xmd /proc/xtratum | cut -d ')' -f 1 | cut -c 2- | sort -r`

	for id in $domslist
	do
		$unloader -id $id 
	done
}

mod_unload()
{
	rmmod rtfifo 2>/dev/null
	rmmod serial 2>/dev/null
	rmmod xm 2>/dev/null
}

dom_load()
{
	$loader $1
}

mod_load()
{
	insmod $XM_PATH/xm.ko 2>/dev/null
	insmod $XM_PATH/devs/serial/serial.ko 2>/dev/null
	insmod $XM_PATH/devs/rtfifo/rtfifo.ko 2>/dev/null
}

if [ -z $1 ] ; then
	echo "ERROR:"
	echo " xmcmd OPTION"
	echo "  OPTION:"
	echo "   -u unloader domain"
	echo "   -l loader domain"
elif [ $1 = "-u" ]; then
	dom_unload
	mod_unload
elif [ $1 = "-l" ]; then
	if [ -z $2 ]; then
		mod_load
	else
		dom_load $2
	fi
fi



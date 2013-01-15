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
	
	domslist=`grep prtk /proc/xtratum | cut -d ')' -f 1 | cut -c 2- | sort -r`

	for id in $domslist
	do
		$unloader -id $id 
	done
}

mod_unload()
{
	rmmod lshm 2>/dev/null
	rmmod luart 2>/dev/null
	rmmod lfifo 2>/dev/null
	rmmod shm 2>/dev/null
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
	insmod $XM_PATH/devs/shm/shm.ko 2>/dev/null
	insmod $XM_PATH/devs/shm/linux/lshm.ko 2>/dev/null
	insmod $XM_PATH/devs/serial/linux/luart.ko 2>/dev/null
	insmod $XM_PATH/devs/rtfifo/linux/lfifo.ko 2>/dev/null
}

create_fifo_device()
{
	count=15
	let minor=0
	
	while [ $minor -le $count ]; do
		devs=/dev/rtf$minor
		if [ -e $devs ]; then
			let minor=minor+1
			continue
		else
			if ! mknod $devs c 150 $minor > /dev/null 2>&1; then
				echo "<FIFO> create fifo device error!"
				exit 1
			fi
		fi
	
		let minor=minor+1
	done
}


create_shm_device()
{
	count=16
	let minor=0
	
	while [ $minor -le $count ]; do
		devs=/dev/shm$minor
		if [ -e $devs ]; then
			let minor=minor+1
			continue
		else
			if ! mknod $devs c 156 $minor > /dev/null 2>&1; then
				echo "<LINUX SHM> create LSHM device error!"
				exit 1
			fi
		fi
	
		let minor=minor+1
	done
}

create_tty_device()
{
	mknod /dev/VttyS c 240 0 > /dev/null 2>&1
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
		create_fifo_device
		create_tty_device
		create_shm_device
		mod_load
	else
		dom_load $2
	fi
fi



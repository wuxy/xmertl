#! /bin/bash


source `dirname $0`/Config.scr

LINUX_UART_PATH=$XM_PATH/devs/serial/linux
XM_BIN_PATH=$XM_PATH/user_tools/scripts
export PATH=$PATH:$XM_BIN_PATH

load_device()
{
	xmcmd.sh -l
	names=`grep serial /proc/modules | cut -d ' ' -f 1`

	for file in $names
	do
		if [ $file = "serial" ]; then
			insmod $LINUX_UART_PATH/luart.ko &2>/dev/null
		fi		
	done
}

create_device()
{
	if [ ! -e /dev/VttyS ]; then
		mknod /dev/VttyS c 240 0
	fi
}

unload_device()
{
	rmmod luart &2>/dev/null
	xmcmd.sh -u
}

if [ -z $1 ]; then
	echo luart.sh {-u, -l}
elif [ $1 = "-l" ]; then

	load_device
	create_device

elif [ $1 == "-u" ]; then

	unload_device

fi

#! /bin/bash
#
# XMeRTL components test tool
#


XMERTL_ROOT=..

LOADER=${XMERTL_ROOT}/xtratum/user_tools/xmloader/loader.xm
UNLOADER=${XMERTL_ROOT}/xtratum/user_tools/xmloader/unloader.xm
XMCMD=${XMERTL_ROOT}/xtratum/user_tools/scripts/xmcmd.sh

function env_check()
{
	err=0;

	echo -n "loader.xm checking ....";
	if `which loader.xm`; then
		LOADER=`which loader.xm`;
	fi

	if [ -e ${LOADER} ]; then
		echo "DONE"; 
	else
		err=1;
		echo "Failure";
	fi
	
	echo -n "unloader.xm checking ....";

	if `which unloader.xm`; then
		LOADER=`which unloader.xm`;
	fi
	if [ -e ${UNLOADER} ]; then
		echo "DONE";
	else 
		err=1;
		echo "Failure";
	fi
	
	echo -n "xmcmd.sh checking ....";
	if `which xmcmd.sh`; then
		LOADER=`which xmcmd.sh`;
	fi
	if [ -e ${XMCMD} ]; then
		echo "DONE";
	else
		err=1;
		echo "Failure";
	fi

	if [ $err -eq 1 ]; then
		echo "ERROR: You should check XMeRTL root path!";
		exit 0;
	fi
}

function test_exit()
{
	unload_all;

	exit $1;
}

function xtratum_test()
{
	echo "XtratuM Testing  ......";

	if [ -x xtratum_tests/test1.xmd ]; then
		if ! ${LOADER} xtratum_tests/test1.xmd; then
			echo "XtratuM testing FAILURE!";
			test_exit -1;
		fi

		echo "XtratuM testing PASS!";
	fi
	
}

function partikle_test()
{
	echo "PaRTiKle Testing ......";

	if [ -x partikle_tests/test1.prtk ]; then
		if ! ${LOADER} partikle_tests/test1.prtk; then
			echo "PaRTiKle testing FAILURE!";
			test_exit -1;
		fi

		echo "PaRTiKle testing PASS!";
	fi
	
	
}

function fifo_test()
{
	echo "XM/FIFO Testing  ......";
	
	if [ -x fifo_tests/reader.prtk ] && [ -x fifo_tests/writer.prtk ]; then
		if ! ${LOADER} fifo_tests/writer.prtk; then
                        echo "XM/FIFO testing FAILURE!";
                        test_exit 0;
                elif ! ${LOADER} fifo_tests/reader.prtk; then
                        echo "XM/FIFO testing FAILURE!";
			test_exit 0;
		fi
	
		# reader will print the fifo testing result based on result.
	
		dmesg | grep "XM/FIFO testing";
	fi
}

function shm_test()
{
	echo "XM/SHM Testing   ......";
	
	if [ -x shm_tests/reader.prtk ] && [ -x shm_tests/writer.prtk ]; then
		if ! ${LOADER} shm_tests/writer.prtk; then
                        echo "XM/SHM testing FAILURE!";
                        test_exit 0;
                elif ! ${LOADER} fifo_tests/reader.prtk; then
                        echo "XM/SHM testing FAILURE!";
			test_exit 0;
		fi
	
		# reader will print the fifo testing result based on result.
	
		dmesg | grep "XM/SHM testing";
	fi
}


function trace_test()
{	
	echo "XM/Trace Testing ......";

	trace_tests/tracer;

	if [ -x ../xtratum/user_tools/examples/tracer/tracer.xmd ]; then
		if ! ${LOADER} ../xtratum/user_tools/examples/tracer/tracer.xmd; then
			echo "XMTrace testing FAILURE! =>  could not load tracer example";
			test_exit -1;
		fi

		echo "XMTrace testing PASS!";
	fi

}

function load_all()
{
	
	${XMCMD} -l	
}

function unload_all()
{
	${XMCMD} -u	
}

function help()
{
	echo "start_test.sh used test the xmertl components";
	echo "";
	echo "USAGE:";
	echo "  start_test.sh [options]";
	echo "";
	echo "options:";
	echo "     Non option will test all components";
	echo "";
	echo "  -t [xtratum|partikle|fifo|shm|trace]";
	echo "     xtratum: xtratum test";
	echo "     partikle: partikle test";
	echo "     fifo: fifo test";
	echo "     shm: shm test";
	echo "     trace: trace test";

	exit 0;	
}

if [ $# -ne 0 ] && [ $1 != "-t" ]; then
	help;
fi

if [ ! -e .mk.xmertl.log ]; then
	echo "Make should be executed before testing!"
	exit 0;
fi 

env_check;


if [ $# -eq 0 ]; then
	xtratum_test ;
	partikle_test;
	fifo_test    ;
	shm_test     ;
	trace_test   ;
elif [ $1 = "-t" ]; then
	case $2 in
		xtratum	)	xtratum_test  ;;
		partikle)	partikle_test ;;
		fifo 	)	fifo_test     ;;
		shm 	) 	shm_test      ;;
		trace 	)	trace_test    ;;
	esac

else
	help;
fi

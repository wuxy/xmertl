#!/bin/sh
# author: Salva Peiró (speiro@ai2.upv.es)
# this script works in two phases:
# run) first runs the tests (from $pts_dir) and stores the results (on $pts_rdir) and 
# bea) next the results are beautified to build a table.

# to work the 3 following vars need to point to the correct location:

pts_dir=$(pwd)		# location of posixtestsuite
pts_rdir=/tmp/tab	# location of posixtestsuite results dir
sus_dir=/usr/susv3/	# location of single unix specification

#testlist=$(ls -d conformance/interfaces/*) # all tests
testlist=$(sed '/^$/d; /^#.*/d; s|^|conformance/interfaces/|' ep51_profile) # only EP51 tests

function skip_unexisting () {
	for f in $*; do
		if test -d $f; then
			echo $f
		fi
	done
}

# skip unexisting tests
testlist=$(skip_unexisting $testlist)

case $1 in
run) # run posix tests
	for f in  $testlist; do #functions
		echo run $f
		> $pts_dir/logfile						# clean up logfile
		mkdir -p $pts_rdir/$f;					# set up $pts_dir
		POSIX_TARGET=$f make > $pts_rdir/$f/log	# save test execution log
		cp $pts_dir/logfile $pts_rdir/$f/		# save full log
	done;
	;;
bea) # beautify output

	echo '<html><table border="1">'
	echo '<thead><tr>Posix Test Suite results table:'\
		'<br>generated '$(date)' by '$USER@$HOSTNAME'</thead>'
	echo '<tbody>'
	for f in $testlist; do #functions
		echo '<tr>'
		echo '<th>'
		echo '<a href=file://'$sus_dir/functions/$(basename $f)'.html>'$(basename $f)'</a>'
		echo '<a href=file:///'$pts_rdir/$f/logfile'>log</a>'
		
		ftests=$(find $pts_dir/$f/ | grep -v svn | grep '[a-zA-Z0-9-]\+\.c')
		for t in $ftests; do #tests
			cfile=$(basename $t)
			fname=$(echo $cfile|sed 's/\.c$//')
			echo '<td>'
			echo '<a href=file:///'$t'>'$cfile'</a>'
			echo '<a href=file:///'$pts_rdir/$f/log'>log</a><br>'
			
			grep '/'$fname':'  $pts_rdir/$f/log |
				sed 's|'$f/'\(speculative/\)\?'$fname': ||g; s/execution/exec/; s/ //; s/$/<br>/'
			echo '</td>'
		done
		echo '</tr>'
	done;
	echo '</tbody></table></html>'
	;;
sum)
	
	;;
all)
	$0 run
	$0 bea > /tmp/tab.html
	;;
*)
	echo "usage: $(basename $0) [run|bea|all] {> /path/to/tab.html}"
	exit 1
	;;
esac

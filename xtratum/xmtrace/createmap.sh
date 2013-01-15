#!/bin/bash

MAPFILE=MySystem.map

if [ "x$1" != "x" ] ; then
  if [ -e $1 ] ; then
     echo File exists will not overwrite - using $MAPFILE !
  else
     echo using $1 as map file
     MAPFILE=$1
  fi
else
  # if its the default name we can overwrite it
  echo using default mapfile: $MAPFILE
fi

# remove
#  a - obsolte symbol value
#  U - undefined symbols
#  w - week symbols - might be overloaded
#  and remove crc entries
#  the list MUST be sorted for the KFT tools to work properly


grep -v '\( [aUw] \)\|\(__crc_\)\|\( \$[adt]\)' /proc/kallsyms  | \
       sort | \
       uniq | \
       cut -f 1 > $MAPFILE



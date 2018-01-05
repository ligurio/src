#!/bin/sh -

PATH="/bin:/usr/bin:/usr/local/bin/:."
ED=$1
OBJDIR=$2
TESTDIR=$3
TEST=$4

[ ! -x $ED ] && { echo "$ED: cannot execute"; exit 1; }
[ ! -e $OBJDIR/$TEST ] && { echo "$TEST: does not exist"; exit 1; }

# *.red scripts must exit with non-zero status
# *.ed scripts must exit with zero status
ext=$(echo $TEST | cut -d'.' -f2)
if [[ "$ext" == "ed" ]]; then
  exit 0
  base=`$ED - \!"echo $i" <<-EOF
    	s/\..*
		EOF`
  if $OBJDIR/$base.ed; then
	if cmp -s $TESTDIR/$base.o $TESTDIR/$base.r; then :; else
		echo "*** Output $TESTDIR/$base.o of script $i is incorrect ***"
		exit 1
	fi
  else
    echo "*** The script $i exited abnormally ***"
    exit 1
  fi
else
  if $i; then
    echo "*** The script $i exited abnormally  ***"
    exit 1
  fi
fi

#!/bin/sh -
#	$OpenBSD: mkscripts.sh,v 1.2 1996/06/23 14:20:08 deraadt Exp $
#	$NetBSD: mkscripts.sh,v 1.10 1995/04/23 10:07:36 cgd Exp $
#
# This script generates ed test scripts (.ed) from .t files

PATH="/bin:/usr/bin:/usr/local/bin/:."
ED=$1
[ ! -x $ED ] && { echo "$ED: cannot execute"; exit 1; }
OBJ="/usr/src/regress/bin/ed/obj"
TESTS="/usr/src/bin/ed/test"

for i in *.t; do
#	base=${i%.*}
#	base=`echo $i | sed 's/\..*//'`
#	base=`expr $i : '\([^.]*\)'`
#	(
#	echo "#!/bin/sh -"
#	echo "$ED - <<\EOT"
#	echo "r $base.d"
#	cat $i
#	echo "w $base.o"
#	echo EOT
#	) >$base.ed
#	chmod +x $base.ed
# The following is pretty ugly way of doing the above, and not appropriate 
# use of ed  but the point is that it can be done...
	base=`$ED - \!"echo $i" <<-EOF
		s/\..*
	EOF`
	$ED - <<-EOF
		a
		#!/bin/sh -
		$ED - <<\EOT
		H
		r $TESTS/$base.d
		w $OBJ/$base.o
		EOT
		.
		-2r $i
		w $OBJ/$base.ed
		!chmod +x $OBJ/$base.ed
	EOF
done

for i in *.err; do
#	base=${i%.*}
#	base=`echo $i | sed 's/\..*//'`
#	base=`expr $i : '\([^.]*\)'`
#	(
#	echo "#!/bin/sh -"
#	echo "$ED - <<\EOT"
#	echo H
#	echo "r $base.err"
#	cat $i
#	echo "w $base.o"
#	echo EOT
#	) >$base-err.ed
#	chmod +x $base-err.ed
# The following is pretty ugly way of doing the above, and not appropriate 
# use of ed  but the point is that it can be done...
	base=`$ED - \!"echo $i" <<-EOF
		s/\..*
	EOF`
	$ED - <<-EOF
		a
		#!/bin/sh -
		$ED - <<\EOT
		H
		r $TESTS/$base.err
		w $OBJ/$base.o
		EOT
		.
		-2r $i
		w $OBJ/${base}.red
		!chmod +x $OBJ/${base}.red
	EOF
done

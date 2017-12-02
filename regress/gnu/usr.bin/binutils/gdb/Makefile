# gdb testsuite requires dejagnu package

GDBDIR= ${.CURDIR}/../../../../../gnu/usr.bin/binutils/gdb/testsuite

REGRESS_TARGETS= run-regress-gdb
CLEANFILES= gdb.sum gdb.log

run-regress-gdb:
	cd ${GDBDIR} && sh ./configure && runtest -tool gdb GDB=/usr/bin/gdb

.include <bsd.regress.mk>

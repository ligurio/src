# gdb testsuite requires dejagnu package

GDBDIR= ${.CURDIR}/../../../../../gnu/usr.bin/binutils/gdb/testsuite

GDB?= /usr/bin/gdb
REGRESS_TARGETS= run-regress-gdb
CLEANFILES= gdb.sum gdb.log

run-regress-gdb:
	cd ${GDBDIR} && sh ./configure && runtest -tool gdb GDB=${GDB}

.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

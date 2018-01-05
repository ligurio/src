# $OpenBSD $

TESTDIR= ${.CURDIR}/../../../bin/ed/test/
OBJDIR= ${.CURDIR}/obj
ED= /bin/ed
SHELL= /bin/sh
CLEANFILES+= ${OBJDIR}/*.red ${OBJDIR}/*.ed ${OBJDIR}/*.o

ARGS!= cd ${OBJDIR} && ls *.{ed,red}
TARGETS?= ${ARGS}
REGRESS_TARGETS= ${TARGETS:S/^/run-regress-/}
REGRESS_SKIP_TARGETS+= run-regress-eq.ed \
				run-regress-a1.ed \
				run-regress-i1.ed \
				run-regress-k1.ed \
				run-regress-r1.ed

regress: makescripts
.for a in ${ARGS}
run-regress-$a: $a
		@echo '\n======== $a ========'
		@${SHELL} ${.CURDIR}/ckscripts.sh ${ED} ${OBJDIR} ${TESTDIR} $a
.endfor

makescripts:
		@cd ${TESTDIR} && $(SHELL) mkscripts.sh ${ED} ${OBJDIR} "/usr/src/bin/ed/test"
		@cp ${TESTDIR}/e1.t ${TESTDIR}/u.t ${TESTDIR}/r3.t ${OBJDIR}

.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

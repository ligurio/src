# $OpenBSD $

TESTDIR= ${.CURDIR}/../../../bin/ed/test/
OBJDIR= ${.CURDIR}/obj
ED= /bin/ed
SHELL= /bin/sh
CLEANFILES += ${OBJDIR}/*.red ${OBJDIR}/*.ed ${OBJDIR}/*.o

regress: makescripts
ARGS!= cd ${OBJDIR} && ls *.{ed,red}
TARGETS?= ${ARGS}
REGRESS_TARGETS= ${TARGETS:S/^/run-regress-/}
.for a in ${ARGS}
run-regress-$a: $a
		@echo '\n======== $a ========'
		@${SHELL} ${.CURDIR}/ckscripts.sh ${ED} ${OBJDIR} ${TESTDIR} $a
.endfor

makescripts:
		cd ${TESTDIR} && $(SHELL) mkscripts.sh $(ED) ${OBJDIR} ${TESTS}

.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

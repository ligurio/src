#	$OpenBSD $

M4=m4
REGRESS_TARGETS= run_test

run_test:
	${M4} ${.CURDIR}/test.m4 | diff -u - ${.CURDIR}/test.out

.PHONY:	${REGRESS_TARGETS}

.include <bsd.regress.mk>

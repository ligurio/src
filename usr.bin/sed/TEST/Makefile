# $OpenBSD $

SED?=	/usr/bin/sed
SHELL?=	/bin/sh
REGRESS_TARGETS= hanoi math

hanoi:
	${SED} -f ${.CURDIR}/$@.sed ${.CURDIR}/$@.in > $@.out
	diff ${.CURDIR}/$@.expected $@.out

math:
	echo "4+7*3" | ${SED} -f $@.sed > $@.out
	diff ${.CURDIR}/$@.expected $@.out

test:
	${SHELL} sed.test

clean:
	@rm -rf nsed.out sed.out hanoi.out \
		math.out lines1 lines2 lines3 \
		script1 script2

.include <bsd.regress.mk>


# $OpenBSD $

EDDIR=	${.CURDIR}/../../../bin/ed/test/
REGRESS_TARGETS= run-regress-ed

run-regress-ed:
	make -C ${EDDIR} check

clean:
	make -C ${EDDIR} clean

.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

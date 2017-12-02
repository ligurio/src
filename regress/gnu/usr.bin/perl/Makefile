PERLDIR= ${.CURDIR}/../../../../gnu/usr.bin/perl/

REGRESS_TARGETS= run-regress-perl

run-regress-perl:
	#${MAKE} -C ${PERLDIR} -f Makefile.bsd-wrapper
	${MAKE} -C ${PERLDIR} -f Makefile.bsd-wrapper test

clean:
	${MAKE} -C ${PERLDIR} clean

.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

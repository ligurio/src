PERLDIR= ${.CURDIR}/../../../../gnu/usr.bin/perl/

REGRESS_TARGETS= run-regress-perl

run-regress-perl:
	make -C ${PERLDIR}
	make -C ${PERLDIR} test

clean:
	make -C ${PERLDIR} clean

.include <bsd.regress.mk>

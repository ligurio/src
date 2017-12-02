# $OpenBSD $

REGRESS_TARGETS= run_test
CLEANFILES= ctags.out

run_test:
	ctags -f ${.CURDIR}/ctags.out ${.CURDIR}/ctags.test && \
		cmp -s ${.CURDIR}/ctags.out ${.CURDIR}/ctags.tags

.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

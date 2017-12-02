# $OpenBSD $

CTAGSDIR=	${.CURDIR}/../../../usr.bin/ctags/test/
.PATH:		${CTAGSDIR}

REGRESS_TARGETS = run-regress-ctags
CLEANFILES= ${CTAGSDIR}/ctags.out

run-regress-ctags:
	ctags -f ${CTAGSDIR}/ctags.out ${CTAGSDIR}/ctags.test && \
		cmp -s ${CTAGSDIR}/ctags.out ${CTAGSDIR}/ctags.tags

.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

# $OpenBSD $

KEYNOTEDIR=	${.CURDIR}/../../../lib/libkeynote/testsuite/
.PATH:		${KEYNOTEDIR}

REGRESS_TARGETS = run-regress-keynote

run-regress-keynote:
	keynote verify -e ${KEYNOTEDIR}/test-env \
               -r false,maybe,probably,true \
               -k ${KEYNOTEDIR}/auth1 -k ${KEYNOTEDIR}auth2  \
               -k ${KEYNOTEDIR}/auth3 -k ${KEYNOTEDIR}/auth4  \
               -l ${KEYNOTEDIR}/test-assertion1 \
               -l ${KEYNOTEDIR}/test-assertion2 \
               -l ${KEYNOTEDIR}/test-assertion3 \
               -l ${KEYNOTEDIR}/test-assertion4 \
               -l ${KEYNOTEDIR}/test-assertion5 \
               -l ${KEYNOTEDIR}/test-assertion6 \
               -l ${KEYNOTEDIR}/test-assertion7

#.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

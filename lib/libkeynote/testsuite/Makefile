#	$OpenBSD $

REGRESS_TARGETS = run_test

run_test:
	keynote verify -e test-env \
               -r false,maybe,probably,true \
               -k auth1 -k auth2  \
               -k auth3 -k auth4  \
               -l test-assertion1 \
               -l test-assertion2 \
               -l test-assertion3 \
               -l test-assertion4 \
               -l test-assertion5 \
               -l test-assertion6 \
               -l test-assertion7

.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

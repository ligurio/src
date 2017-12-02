
PROG=asn1test
REGRESS_TARGETS= testss tcrl testenc testgen
CLEANFILES+= .rnd certCA.srl certCA.ss certP1.srl certP1.ss certP2.ss \
		certU.srl certU.ss cms.err cms.out keyCA.ss keyP1.ss \
		keyP2.ss keyU.ss req2CA.ss reqCA.ss reqP1.ss reqP2.ss \
		reqU.ss testkey.pem testreq.pem tmp.bctest tmp_intP1.ss \
		tmp_intP2.ss

# bctest
# cms-examples.pl
# cms-test.pl -- fix?
#
# testca -- requires opensslwrap
# methtest.c -- meth.h is absent
# r160test.c -- empty
# test_padlock -- fix or remove?
# test_aesni -- remove because engine command is not available

# Requires sharedlibs:
# pkits-test.pl 
# testssl testsslproxy testtsa tpkcs7
# tpkcs7d treq trsa tsid tx509 tx509

testss:
	${SHELL} ./testss

tcrl:
	${SHELL} ./tcrl

testenc:
	${SHELL} ./testenc

testgen:
	${SHELL} ./testgen

.PHONY: ${REGRESS_TARGETS}

.include <bsd.regress.mk>

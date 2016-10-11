##############################################################################
#									     #
# Software supplement to the book "Cryptography in C and C++"                #
# by Michael Welschenbach                                                    #
#                                                                            #
# makefile.asm for running the FLINT/C testsuite under Unix and Linux        #
# with support of assembler functions                                        #
# Authors: Jan-Peter Wilhelms, Michael Welschenbach			     #
#									     #
# Copyright (C) 1998-2005 by Michael Welschenbach                            #
# Copyright (C) 1998-2005 by Springer-Verlag Berlin, Heidelberg              #
# Copyright (C) 2001-2005 by Apress L.P., Berkeley, CA                       #
# Copyright (C) 2002-2005 by Wydawnictwa MIKOM, Poland                       #
# Copyright (C) 2002-2005 by PHEI, P.R.China                                 #
# Copyright (C) 2002-2005 by InfoBook, Korea                                 #
# Copyright (C) 2002-2005 by Triumph Publishing, Russia                      #
#                                                                            #
# All Rights Reserved                                                        #
#                                                                            #
# The software may be used for noncommercial purposes and may be altered,    #
# as long as the following conditions are accepted without any               #
# qualification:                                                             #
#                                                                            #
# (1) All changes to the sources must be identified in such a way that the   #
#     changed software cannot be misinterpreted as the original software.    #
#                                                                            #
# (2) The statements of copyright may not be removed or altered.             #
#                                                                            #
# (3) The following DISCLAIMER is accepted:                                  #
#                                                                            #
# DISCLAIMER:                                                                #
#                                                                            #
# There is no warranty for the software contained in this distribution, to   #
# the extent permitted by applicable law. The copyright holders provide the  #
# software `as is' without warranty of any kind, either expressed or         #
# implied, including, but not limited to, the implied warranty of fitness    #
# for a particular purpose. The entire risk as to the quality and            #
# performance of the program is with you.                                    #
#                                                                            #
# In no event unless required by applicable law or agreed to in writing      #
# will the copyright holders, or any of the individual authors named in      #
# the source files, be liable to you for damages, including any general,     #
# special, incidental or consequential damages arising out of any use of     #
# the software or out of inability to use the software (including but not    #
# limited to any financial losses, loss of data or data being rendered       #
# inaccurate or losses sustained by you or by third parties as a result of   #
# a failure of the software to operate with any other programs), even if     #
# such holder or other party has been advised of the possibility of such     #
# damages.                                                                   #
#                                                                            #
#============================================================================#
#									     #
#      Usage: 						   		     #
#      ------    							     #
#      make w/o parameters runs the whole testsuite			     #
#      make with one of the parameters					     #
#									     #
#        rnd reg bas add sub mul kar sqr div madd msub mmul msqr shift       #
#	 set bool iroot gcd mexp con ari chin xgcd root proot ripe sha1      #
#									     #
#      runs the single test program denoted by the parameter. 		     #
#									     #
##############################################################################

test:	rnd reg bas add sub mul kar sqr div madd msub mmul msqr shift set bool iroot gcd mexp ripe sha1 sha256 ari chin xgcd root prrt

rnd:	flint.o testrand
	testrand 2> err

reg:	flint.o testreg
	testreg 2>> err

bas:	flint.o testbas
	testbas 2>> err

add:	flint.o testadd
	testadd 2>> err

sub:	flint.o testsub
	testsub 2>> err

mul:	flint.o testmul
	testmul 2>> err

kar:	flint.o ../src/kmul.c testkar.c
	gcc -DFLINT_ASM -Wall -ansi -O2 -I../src -L../lib -o testkar testkar.c ../src/kmul.c flint.o  -lflint
	testkar 2>> err
	
sqr:	flint.o testsqr
	testsqr 2>> err

div:	flint.o testdiv
	testdiv 2>> err

madd:	flint.o testmadd
	testmadd 2>> err

msub:	flint.o testmsub
	testmsub 2>> err

mmul:	flint.o testmmul
	testmmul 2>> err

msqr:	flint.o testmsqr
	testmsqr 2>> err

shift:	flint.o testshft
	testshft 2>> err

set:	flint.o testset
	testset 2>> err

bool:	flint.o testbool
	testbool 2>> err

iroot:	flint.o testiroo
	testiroo 2>> err

gcd:	flint.o testgcd
	testgcd 2>> err

mexp:	flint.o testmexp
	testmexp 2>> err

con:	flintpp.o flint.o testcnst
	testcnst 2>> err

ari:	flintpp.o flint.o testari
	testari 2>> err

chin:	flintpp.o flint.o testchin
	testchin 2>> err

xgcd:	flintpp.o flint.o testxgcd
	testxgcd 2>> err

root:	flintpp.o flint.o random.o entropy.o aes.o ripemd.o sha1.o randompp.o
	gcc -Wall -ansi -O2 -I../src -DFLINT_ASM -L../lib -o testroot testroot.cpp random.o entropy.o aes.o ripemd.o sha1.o randompp.o flint.o flintpp.o -lflint -lstdc++
	testroot 2>> err

prrt:	flintpp.o flint.o testprrt
	testprrt 2>> err

ripe:	../src/ripemd.c testripe.c
	gcc -Wall -ansi -O2 -I../src -o testripe testripe.c ../src/ripemd.c
	testripe 2>> err

sha1:	../src/sha1.c testsha1.c
	gcc -Wall -ansi -O2 -I../src -o testsha1 testsha1.c ../src/sha1.c
	testsha1 2>> err

sha256:	../src/sha256.c testsha256.c
	gcc -Wall -ansi -O2 -I../src -o testsha256 testsha256.c ../src/sha256.c
	testsha256 2>> err

flint.o: ../src/flint.c
	gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM ../src/flint.c

flintpp.o: ../src/flintpp.cpp
	gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM ../src/flintpp.cpp

random.o: ../src/random.c
	gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM ../src/random.c

entropy.o: ../src/entropy.c
	gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM ../src/entropy.c

aes.o: ../src/aes.c
	gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM ../src/aes.c

ripemd.o: ../src/ripemd.c
	gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM ../src/ripemd.c

sha1.o: ../src/sha1.c
	gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM ../src/sha1.c

sha256.o: ../src/sha256.c
	gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM ../src/sha256.c

randompp.o: ../src/randompp.cpp
	gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM ../src/randompp.cpp

	
.SUFFIXES: .o .c .cpp

.c:
	gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o $@ $< flint.o -lflint

.cpp:
	gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o $@ $< flint.o flintpp.o -lflint -lstdc++


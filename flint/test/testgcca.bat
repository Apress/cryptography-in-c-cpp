@echo off
REM **************************************************************************
REM Software supplement to the book "Cryptography in C and C++"
REM by Michael Welschenbach, published by Apress Berkeley CA, 2001
REM
REM File testgcca.bat
REM Batch file for running the FLINT/C testsuite with gcc under Windows
REM with support of assembler functions.
REM Usage: testgcca [reg bas add sub mul sqr div madd msub mmul msqr shift set
REM                  bool iroot gcd mexp ripe sha1 con ari xgcd root chin prrt]
REM
REM Copyright (C) 1998-2005 by Michael Welschenbach
REM Copyright (C) 2001-2005 by Springer-Verlag Berlin, Heidelberg
REM Copyright (C) 2001-2005 by Apress L.P., Berkeley, CA
REM Copyright (C) 2002-2005 by Wydawnictwa MIKOM, Poland
REM Copyright (C) 2002-2005 by PHEI, P.R.China
REM Copyright (C) 2002-2005 by InfoBook, Korea
REM Copyright (C) 2002-2005 by Triumph Publishing, Russia
REM
REM All Rights Reserved
REM
REM The software may be used for noncommercial purposes and may be altered,
REM as long as the following conditions are accepted without any
REM qualification:
REM
REM (1) All changes to the sources must be identified in such a way that the
REM     changed software cannot be misinterpreted as the original software.
REM
REM (2) The statements of copyright may not be removed or altered.
REM
REM (3) The following DISCLAIMER is accepted:
REM
REM DISCLAIMER:
REM
REM There is no warranty for the software contained on this CD-ROM, to the
REM extent permitted by applicable law. The copyright holders provide the
REM software `as is' without warranty of any kind, either expressed or
REM implied, including, but not limited to, the implied warranty of fitness
REM for a particular purpose. The entire risk as to the quality and
REM performance of the program is with you.
REM
REM In no event unless required by applicable law or agreed to in writing
REM will the copyright holders, or any of the individual authors named in
REM the source files, be liable to you for damages, including any general,
REM special, incidental or consequential damages arising out of any use of
REM the software or out of inability to use the software (including but not
REM limited to any financial losses, loss of data or data being rendered
REM inaccurate or losses sustained by you or by third parties as a result of
REM a failure of the software to operate with any other programs), even if
REM such holder or other party has been advised of the possibility of such
REM damages. 
REM **************************************************************************


gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM -o flint.o ../src/flint.c
gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM -o flintpp.o ../src/flintpp.cpp
gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM -o random.o ../src/random.c
gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM -o entropy.o ../src/entropy.c
gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM -o aes.o ../src/aes.c
gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM -o ripemd.o ../src/ripemd.c
gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM -o sha1.o ../src/sha1.c
gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM -o sha1.o ../src/sha256.c
gcc -c -O2 -Wall -ansi -I../src -DFLINT_ASM -o randompp.o ../src/randompp.cpp
gcc -c ../src/asm/cygwin/*.s

if not %1~ == ~  goto %1

REM Test of linear congruential generator
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testrand.exe testrand.c flint.o div.o mult.o umul.o sqr.o
testrand
if not errorlevel 0 goto err

REM Test of BBS-Generator
:bbs
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testbbs.exe testbbs.c random.o entropy.o aes.o ripemd.o sha1.o flint.o div.o mult.o umul.o sqr.o
REM testbbs takes a long time...
REM testbbs
if not errorlevel 0 goto err

REM Test of registers
:reg
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testreg.exe testreg.c flint.o div.o mult.o umul.o sqr.o
testreg
if not errorlevel 0 goto err

REM Test of base functions
:bas
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testbas.exe testbas.c flint.o div.o mult.o umul.o sqr.o
testbas
if not errorlevel 0 goto err

REM Test of addition
:add
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testadd.exe testadd.c flint.o div.o mult.o umul.o sqr.o
testadd
if not errorlevel 0 goto err

REM Test of subtraction
:sub
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testsub.exe testsub.c flint.o div.o mult.o umul.o sqr.o
testsub
if not errorlevel 0 goto err

REM Test of multiplication
:mul
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testmul.exe testmul.c flint.o div.o mult.o umul.o sqr.o
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testkar.exe testkar.c ../src/kmul.c flint.o div.o mult.o umul.o sqr.o
testmul
if not errorlevel 0 goto err
testkar
if not errorlevel 0 goto err

REM Test of Squaring
:sqr
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testsqr.exe testsqr.c flint.o div.o mult.o umul.o sqr.o
testsqr
if not errorlevel 0 goto err

REM Test of integer division
:div
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testdiv.exe testdiv.c flint.o div.o mult.o umul.o sqr.o
testdiv
if not errorlevel 0 goto err

REM Test of modular addition
:madd
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testmadd.exe testmadd.c flint.o div.o mult.o umul.o sqr.o
testmadd
if not errorlevel 0 goto err

REM Test of modular subtraction
:msub
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testmsub.exe testmsub.c flint.o div.o mult.o umul.o sqr.o
testmsub
if not errorlevel 0 goto err

REM Test of modular multiplication
:mmul
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testmmul.exe testmmul.c flint.o div.o mult.o umul.o sqr.o
testmmul
if not errorlevel 0 goto err

REM Test of modular squaring
:msqr
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testmsqr.exe testmsqr.c flint.o div.o mult.o umul.o sqr.o
testmsqr
if not errorlevel 0 goto err

REM Test of shift operations
:shift
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testshft.exe testshft.c flint.o div.o mult.o umul.o sqr.o
testshft
if not errorlevel 0 goto err

REM Test of functions for bit access
:set
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testset.exe testset.c flint.o div.o mult.o umul.o sqr.o
testset
if not errorlevel 0 goto err

REM Test of Boolean functions
:bool
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testbool.exe testbool.c flint.o div.o mult.o umul.o sqr.o
testbool
if not errorlevel 0 goto err

REM Test of calculation of integer square roots
:iroot
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testiroo.exe testiroo.c flint.o div.o mult.o umul.o sqr.o
testiroo
if not errorlevel 0 goto err

REM Test of gcd, xgcd
:gcd
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testgcd.exe testgcd.c flint.o div.o mult.o umul.o sqr.o
testgcd
if not errorlevel 0 goto err

REM Test of modular exponentiation 
:mexp
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testmexp.exe testmexp.c flint.o div.o mult.o umul.o sqr.o
testmexp
if not errorlevel 0 goto err

REM Test of RIPEMD160
:ripe
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testripe.exe testripe.c ../src/ripemd.c
testripe
if not errorlevel 0 goto err

REM Test of SHA-1, SHA-256
:sha1
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testsha1.exe testsha1.c sha1.o
testsha1
if not errorlevel 0 goto err
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testsha256.exe testsha256.c sha256.o
testsha256
if not errorlevel 0 goto err

REM Test of LINT constructors
:con
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testcnst.exe testcnst.cpp flint.o div.o mult.o umul.o sqr.o flintpp.o -lstdc++
testcnst
if not errorlevel 0 goto err

REM Test of LINT arithmetic
:ari
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testari.exe testari.cpp flint.o div.o mult.o umul.o sqr.o flintpp.o -lstdc++
testari
if not errorlevel 0 goto err

REM Test of calculation of eXtended Euclidean Algorithm
:xgcd
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testxgcd.exe testxgcd.cpp flint.o div.o mult.o umul.o sqr.o flintpp.o -lstdc++
testxgcd
if not errorlevel 0 goto err

REM Test of LINT-Functions isprime, gcd, jacobi and root
:root
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testroot.exe testroot.cpp random.o entropy.o aes.o ripemd.o sha1.o randompp.o flint.o div.o mult.o umul.o sqr.o flintpp.o -lstdc++
testroot
if not errorlevel 0 goto err

REM Test of LINT-Function chinrest
:chin
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testchin.exe testchin.cpp flint.o div.o mult.o umul.o sqr.o flintpp.o -lstdc++
testchin
if not errorlevel 0 goto err

REM Test of LINT-Function primroot
:prrt
gcc -O2 -Wall -ansi -I../src -L../lib -DFLINT_ASM -o testprrt.exe testprrt.cpp flint.o div.o mult.o umul.o sqr.o flintpp.o -lstdc++
testprrt
if not errorlevel 0 goto err


goto exit

:err
echo Error in test module
quit 255

:exit
*del *.o
echo All tests O.K.


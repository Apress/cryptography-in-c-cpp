@echo off
REM **************************************************************************
REM Software supplement to the book "Cryptography in C and C++"
REM by Michael Welschenbach, published by Apress Berkeley CA, 2001
REM
REM File testgcc.bat
REM Batch file for running the FLINT/C testsuite under DOS and Windows 
REM Usage: testgcc [reg bas add sub mul sqr div madd msub mmul msqr shift set
REM                 bool iroot gcd mexp ripe sha1 con ari xgcd root chin prrt]
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


gcc -c -O2 -Wall -ansi -I../src -o flint.o ../src/flint.c
gcc -c -O2 -Wall -ansi -I../src -o flintpp.o ../src/flintpp.cpp
gcc -c -O2 -Wall -ansi -I../src -o aes.o ../src/aes.c
gcc -c -O2 -Wall -ansi -I../src -o random.o ../src/random.c
gcc -c -O2 -Wall -ansi -I../src -o entropy.o ../src/entropy.c
gcc -c -O2 -Wall -ansi -I../src -o ripemd.o ../src/ripemd.c
gcc -c -O2 -Wall -ansi -I../src -o sha1.o ../src/sha1.c
gcc -c -O2 -Wall -ansi -I../src -o sha1.o ../src/sha256.c
gcc -c -O2 -Wall -ansi -I../src -o randompp.o ../src/randompp.cpp

if not %1~ == ~  goto %1

REM Test of linear congruential generator
gcc -O2 -Wall -ansi -I../src -o testrand.exe testrand.c flint.o
testrand
if not errorlevel 0 goto err

REM Test of BBS-Generator
:bbs
gcc -O2 -Wall -ansi -I../src -o testbbs.exe testbbs.c aes.o random.o entropy.o ripemd.o sha1.o flint.o
REM testbbs takes a long time...
REM testbbs
if not errorlevel 0 goto err

REM Test of registers
:reg
gcc -O2 -Wall -ansi -I../src -o testreg.exe testreg.c flint.o
testreg
if not errorlevel 0 goto err

REM Test of base functions
:bas
gcc -O2 -Wall -ansi -I../src -o testbas.exe testbas.c flint.o
testbas
if not errorlevel 0 goto err

REM Test of addition
:add
gcc -O2 -Wall -ansi -I../src -o testadd.exe testadd.c flint.o
testadd
if not errorlevel 0 goto err

REM Test of subtraction
:sub
gcc -O2 -Wall -ansi -I../src -o testsub.exe testsub.c flint.o
testsub
if not errorlevel 0 goto err

REM Test of multiplication
:mul
gcc -O2 -Wall -ansi -I../src -o testmul.exe testmul.c flint.o
gcc -O2 -Wall -ansi -I../src -o testkar.exe testkar.c ../src/kmul.c flint.o
testmul
if not errorlevel 0 goto err
testkar
if not errorlevel 0 goto err

REM Test of Squaring
:sqr
gcc -O2 -Wall -ansi -I../src -o testsqr.exe testsqr.c flint.o
testsqr
if not errorlevel 0 goto err

REM Test of integer division
:div
gcc -O2 -Wall -ansi -I../src -o testdiv.exe testdiv.c flint.o
testdiv
if not errorlevel 0 goto err

REM Test of modular addition
:madd
gcc -O2 -Wall -ansi -I../src -o testmadd.exe testmadd.c flint.o
testmadd
if not errorlevel 0 goto err

REM Test of modular subtraction
:msub
gcc -O2 -Wall -ansi -I../src -o testmsub.exe testmsub.c flint.o
testmsub
if not errorlevel 0 goto err

REM Test of modular multiplication
:mmul
gcc -O2 -Wall -ansi -I../src -o testmmul.exe testmmul.c flint.o
testmmul
if not errorlevel 0 goto err

REM Test of modular squaring
:msqr
gcc -O2 -Wall -ansi -I../src -o testmsqr.exe testmsqr.c flint.o
testmsqr
if not errorlevel 0 goto err

REM Test of shift operations
:shift
gcc -O2 -Wall -ansi -I../src -o testshft.exe testshft.c flint.o
testshft
if not errorlevel 0 goto err

REM Test of functions for bit access
:set
gcc -O2 -Wall -ansi -I../src -o testset.exe testset.c flint.o
testset
if not errorlevel 0 goto err

REM Test of Boolean functions
:bool
gcc -O2 -Wall -ansi -I../src -o testbool.exe testbool.c flint.o
testbool
if not errorlevel 0 goto err

REM Test of calculation of integer square roots
:iroot
gcc -O2 -Wall -ansi -I../src -o testiroo.exe testiroo.c flint.o
testiroo
if not errorlevel 0 goto err

REM Test of gcd, xgcd
:gcd
gcc -O2 -Wall -ansi -I../src -o testgcd.exe testgcd.c flint.o
testgcd
if not errorlevel 0 goto err

REM Test of modular exponentiation 
:mexp
gcc -O2 -Wall -ansi -I../src -o testmexp.exe testmexp.c flint.o
testmexp
if not errorlevel 0 goto err

REM Test of RIPEMD160
:ripe
gcc -O2 -Wall -ansi -I../src -o testripe.exe testripe.c ../src/ripemd.c
testripe
if not errorlevel 0 goto err

REM Test of SHA-1, SHA-256
:sha1
gcc -O2 -Wall -ansi -I../src -o testsha1.exe testsha1.c sha1.o
testsha1
if not errorlevel 0 goto err
gcc -O2 -Wall -ansi -I../src -o testsha256.exe testsha256.c sha256.o
testsha256
if not errorlevel 0 goto err

REM Test of LINT constructors
:con
gcc -O2 -Wall -ansi -I../src -o testcnst.exe testcnst.cpp flint.o flintpp.o -lstdc++
testcnst
if not errorlevel 0 goto err

REM Test of LINT arithmetic
:ari
gcc -O2 -Wall -ansi -I../src -o testari.exe testari.cpp flint.o flintpp.o -lstdc++
testari
if not errorlevel 0 goto err

REM Test of calculation of eXtended Euclidean Algorithm
:xgcd
gcc -O2 -Wall -ansi -I../src -o testxgcd.exe testxgcd.cpp flint.o flintpp.o -lstdc++
testxgcd
if not errorlevel 0 goto err

REM Test of LINT-Functions isprime, gcd, jacobi and root
:root
gcc -O2 -Wall -ansi -I../src -o testroot.exe testroot.cpp aes.o random.o entropy.o ripemd.o sha1.o randompp.o flint.o flintpp.o -lstdc++
testroot
if not errorlevel 0 goto err

REM Test of LINT-Function chinrest
:chin
gcc -O2 -Wall -ansi -I../src -o testchin.exe testchin.cpp flint.o flintpp.o -lstdc++
testchin
if not errorlevel 0 goto err

REM Test of LINT-Function primroot
:prrt
gcc -O2 -Wall -ansi -I../src -o testprrt.exe testprrt.cpp flint.o flintpp.o -lstdc++
testprrt
if not errorlevel 0 goto err


goto exit

:err
echo Error in test module
quit 255

:exit
echo All tests O.K.


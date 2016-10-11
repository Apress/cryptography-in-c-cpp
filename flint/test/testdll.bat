@echo off
REM **************************************************************************
REM Software supplement to the book "Cryptography in C and C++"
REM by Michael Welschenbach                                          
REM
REM File testdll.bat
REM Batch file for running the FLINT/C testsuite under Windows 
REM Usage: testdll [reg bas add sub mul sqr div madd msub mmul msqr shift set
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


cl -c -O2 -W3 -DFLINT_USEDLL -nologo -I..\src ..\src\flintpp.cpp

if not %1~ == ~  goto %1

REM Test of linear congruential generator
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testrand.c ..\lib\dll\flint.lib
testrand
if not errorlevel 0 goto err

REM Test of BBS-Generator
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testbbs.c ..\lib\dll\flint.lib
REM testbbs takes a long time...
REM testbbs
if not errorlevel 0 goto err

REM Test of registers
:reg
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testreg.c ..\lib\dll\flint.lib
testreg
if not errorlevel 0 goto err

REM Test of base functions
:bas
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testbas.c ..\lib\dll\flint.lib
testbas
if not errorlevel 0 goto err

REM Test of addition
:add
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testadd.c ..\lib\dll\flint.lib
testadd
if not errorlevel 0 goto err

REM Test of subtraction
:sub
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testsub.c ..\lib\dll\flint.lib
testsub
if not errorlevel 0 goto err

REM Test of multiplication
:mul
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testmul.c ..\lib\dll\flint.lib
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testkar.c ..\src\kmul.c ..\lib\dll\flint.lib
testmul
if not errorlevel 0 goto err
testkar
if not errorlevel 0 goto err

REM Test of Squaring
:sqr
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testsqr.c ..\lib\dll\flint.lib
testsqr
if not errorlevel 0 goto err

REM Test of integer division
:div
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testdiv.c ..\lib\dll\flint.lib
testdiv
if not errorlevel 0 goto err

REM Test of modular addition
:madd
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testmadd.c ..\lib\dll\flint.lib
testmadd
if not errorlevel 0 goto err

REM Test of modular subtraction
:msub
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testmsub.c ..\lib\dll\flint.lib
testmsub
if not errorlevel 0 goto err

REM Test of modular multiplication
:mmul
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testmmul.c ..\lib\dll\flint.lib
testmmul
if not errorlevel 0 goto err

REM Test of modular squaring
:msqr
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testmsqr.c ..\lib\dll\flint.lib
testmsqr
if not errorlevel 0 goto err

REM Test of shift operations
:shift
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testshft.c ..\lib\dll\flint.lib
testshft
if not errorlevel 0 goto err

REM Test of functions for bit access
:set
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testset.c ..\lib\dll\flint.lib
testset
if not errorlevel 0 goto err

REM Test of Boolean functions
:bool
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testbool.c ..\lib\dll\flint.lib
testbool
if not errorlevel 0 goto err

REM Test of calculation of integer square roots
:iroot
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testiroo.c ..\lib\dll\flint.lib
testiroo
if not errorlevel 0 goto err

REM Test of gcd, xgcd
:gcd
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testgcd.c ..\lib\dll\flint.lib
testgcd
if not errorlevel 0 goto err

REM Test of modular exponentiation 
:mexp
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testmexp.c ..\lib\dll\flint.lib
testmexp
if not errorlevel 0 goto err

REM Test of RIPED160
:ripe
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testripe.c ..\src\ripemd.c
testripe
if not errorlevel 0 goto err

REM Test of SHA-1, SHA-256
:sha1
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testsha1.c ..\lib\dll\flint.lib
testsha1
if not errorlevel 0 goto err
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testsha256.c ..\lib\dll\flint.lib
testsha256
if not errorlevel 0 goto err

REM Test of LINT constructors
:con
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testcnst.cpp ..\lib\dll\flint.lib flintpp.obj
testcnst
if not errorlevel 0 goto err

REM Test of LINT arithmetic
:ari
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testari.cpp ..\lib\dll\flint.lib flintpp.obj
testari
if not errorlevel 0 goto err

REM Test of calculation of eXtended Euclidean Algorithm
:xgcd
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testxgcd.cpp ..\lib\dll\flint.lib flintpp.obj
testxgcd
if not errorlevel 0 goto err

REM Test of LINT-Functions isprime, gcd, jacobi and root
:root
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testroot.cpp randompp.obj flintpp.obj ..\lib\dll\flint.lib
testroot
if not errorlevel 0 goto err

REM Test of LINT-Function chinrest
:chin
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testchin.cpp ..\lib\dll\flint.lib flintpp.obj
testchin
if not errorlevel 0 goto err

REM Test of LINT-Function primroot
:prrt
cl -O2 -W3 -DFLINT_USEDLL -nologo -I..\src testprrt.cpp ..\lib\dll\flint.lib flintpp.obj
testprrt
if not errorlevel 0 goto err


goto exit

:err
echo Error in test module
quit 255

:exit
echo All tests OK






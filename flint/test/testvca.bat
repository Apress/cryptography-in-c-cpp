@echo off
REM **************************************************************************
REM Software supplement to the book "Cryptography in C and C++"
REM by Michael Welschenbach                                            
REM
REM File testvca.bat
REM Batch file for running the FLINT/C testsuite with Microsoft Visual C/C++ 
REM Usage: testvca [reg bas add sub mul sqr div madd msub mmul msqr shift set
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


cl -c -O2 -W3 -nologo -I..\src -DFLINT_ASM ..\src\flint.c
cl -c -O2 -W3 -nologo -I..\src -DFLINT_ASM ..\src\random.c
cl -c -O2 -W3 -nologo -I..\src -DFLINT_ASM ..\src\entropy.c
cl -c -O2 -W3 -nologo -I..\src -DFLINT_ASM ..\src\aes.c
cl -c -O2 -W3 -nologo -I..\src -DFLINT_ASM ..\src\ripemd.c
cl -c -O2 -W3 -nologo -I..\src -DFLINT_ASM ..\src\sha1.c
cl -c -O2 -W3 -nologo -I..\src -DFLINT_ASM ..\src\sha256.c
cl -c -O2 -W3 -nologo -I..\src -DFLINT_ASM ..\src\randompp.cpp
cl -c -O2 -W3 -nologo -I..\src -DFLINT_ASM ..\src\flintpp.cpp

if not %1~ == ~  goto %1

REM Test of linear congruential generator
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testrand.c flint.obj ..\lib\flintavc.lib
testrand
if not errorlevel 0 goto err

REM Test of BBS-Generator
:bbs
cl -O2 -DFLINT_ASM -nologo -I..\src testbbs.c random.obj entropy.obj aes.obj ripemd.obj sha1.obj flint.obj ..\lib\flintavc.lib advapi32.lib
REM testbbs dauert sehr lange...
REM testbbs
if not errorlevel 0 goto err

REM Test of registers
:reg
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testreg.c flint.obj ..\lib\flintavc.lib
testreg
if not errorlevel 0 goto err

REM Test of base functions
:bas
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testbas.c flint.obj ..\lib\flintavc.lib
testbas
if not errorlevel 0 goto err

REM Test of addition
:add
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testadd.c flint.obj ..\lib\flintavc.lib
testadd
if not errorlevel 0 goto err

REM Test of subtraction
:sub
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testsub.c flint.obj ..\lib\flintavc.lib
testsub
if not errorlevel 0 goto err

REM Test of multiplication
:mul
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testmul.c flint.obj ..\lib\flintavc.lib
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testkar.c ..\src\kmul.c flint.obj ..\lib\flintavc.lib
testmul
if not errorlevel 0 goto err
testkar
if not errorlevel 0 goto err

REM Test of Squaring
:sqr
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testsqr.c flint.obj ..\lib\flintavc.lib
testsqr
if not errorlevel 0 goto err

REM Test of integer division
:div
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testdiv.c flint.obj ..\lib\flintavc.lib
testdiv
if not errorlevel 0 goto err

REM Test of modular addition
:madd
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testmadd.c flint.obj ..\lib\flintavc.lib
testmadd
if not errorlevel 0 goto err

REM Test of modular subtraction
:msub
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testmsub.c flint.obj ..\lib\flintavc.lib
testmsub
if not errorlevel 0 goto err

REM Test of modular multiplication
:mmul
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testmmul.c flint.obj ..\lib\flintavc.lib
testmmul
if not errorlevel 0 goto err

REM Test of modular squaring
:msqr
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testmsqr.c flint.obj ..\lib\flintavc.lib
testmsqr
if not errorlevel 0 goto err

REM Test of shift operations
:shift
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testshft.c flint.obj ..\lib\flintavc.lib
testshft
if not errorlevel 0 goto err

REM Test of functions for bit access
:set
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testset.c flint.obj ..\lib\flintavc.lib
testset
if not errorlevel 0 goto err

REM Test of Boolean functions
:bool
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testbool.c flint.obj ..\lib\flintavc.lib
testbool
if not errorlevel 0 goto err

REM Test of calculation of integer square roots
:iroot
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testiroo.c flint.obj ..\lib\flintavc.lib
testiroo
if not errorlevel 0 goto err

REM Test of gcd, xgcd
:gcd
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testgcd.c flint.obj ..\lib\flintavc.lib
testgcd
if not errorlevel 0 goto err

REM Test of modular exponentiation 
:mexp
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testmexp.c flint.obj ..\lib\flintavc.lib
testmexp
if not errorlevel 0 goto err

REM Test of RIPEMD160
:ripe
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testripe.c ..\src\ripemd.c
testripe
if not errorlevel 0 goto err

REM Test of SHA-1, SHA-256
:sha1
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testsha1.c sha1.obj
testsha1
if not errorlevel 0 goto err
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testsha256.c sha256.obj
testsha256
if not errorlevel 0 goto err

REM Test of LINT constructors
:con
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testcnst.cpp flint.obj flintpp.obj ..\lib\flintavc.lib 
testcnst
if not errorlevel 0 goto err

REM Test of LINT arithmetic
:ari
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testari.cpp flint.obj flintpp.obj ..\lib\flintavc.lib 
testari
if not errorlevel 0 goto err

REM Test of calculation of eXtended Euclidean Algorithm
:xgcd
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testxgcd.cpp flint.obj flintpp.obj ..\lib\flintavc.lib 
testxgcd
if not errorlevel 0 goto err

REM Test of LINT-Functions isprime, gcd, jacobi and root
:root
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testroot.cpp random.obj entropy.obj aes.obj ripemd.obj sha1.obj randompp.obj flint.obj flintpp.obj ..\lib\flintavc.lib advapi32.lib
testroot
if not errorlevel 0 goto err

REM Test of LINT-Function chinrest
:chin
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testchin.cpp flint.obj flintpp.obj ..\lib\flintavc.lib 
testchin
if not errorlevel 0 goto err

REM Test of LINT-Function primroot
:prrt
cl -O2 -W3 -DFLINT_ASM -nologo -I..\src testprrt.cpp flint.obj flintpp.obj ..\lib\flintavc.lib 
testprrt
if not errorlevel 0 goto err


goto exit

:err
echo Error in test module
quit 255

:exit
echo All tests OK

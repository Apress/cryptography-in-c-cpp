##############################################################################
#									     #
# Software supplement to the book "Cryptography in C and C++"                #
# by Michael Welschenbach                                                    #
#                                                                            #
# makefile.asm for RSA demo with support of assembler functions              #
#                                                                            #
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
##############################################################################

CFLAGS = -c -O2 -Wall -ansi -DFLINT_ASM -I../src -o $@

#Parameters for Unix/Linux
STDCPP=stdc++
EXECUTABLE = -o rsademo
LIBPATH = ../lib
FLINTLIB = flint

#Parameters for DOS, Windows, OS/2
#STDCPP=stdcpp
#STDCPP=stdc++
#EXECUTABLE = -o rsademo.exe
#LIBPATH = ../lib
#FLINTLIB = flinta


rsademo: rsademo.o rsakey.o flintpp.o flint.o random.o entropy.o aes.o ripemd.o sha1.o randompp.o
	gcc $(EXECUTABLE) rsademo.o rsakey.o random.o entropy.o aes.o ripemd.o sha1.o randompp.o flintpp.o flint.o -l$(FLINTLIB) -l$(STDCPP)

rsademo.o: rsademo.cpp
	gcc $(CFLAGS) rsademo.cpp

rsakey.o: rsakey.cpp
	gcc $(CFLAGS) rsakey.cpp

flintpp.o: ../src/flintpp.cpp
	gcc $(CFLAGS) ../src/flintpp.cpp

flint.o: ../src/flint.c
	gcc $(CFLAGS) ../src/flint.c

ripemd.o: ../src/ripemd.c
	gcc $(CFLAGS) ../src/ripemd.c

randompp.o: ../src/randompp.cpp
	gcc $(CFLAGS) ../src/randompp.cpp

random.o: ../src/random.c
	gcc $(CFLAGS) ../src/random.c

aes.o: ../src/aes.c
	gcc $(CFLAGS) ../src/aes.c

sha1.o: ../src/sha1.c
	gcc $(CFLAGS) ../src/sha1.c

entropy.o: ../src/entropy.c
	gcc $(CFLAGS) ../src/entropy.c


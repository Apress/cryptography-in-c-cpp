CFLAGS = -v -c -O2 -Wall -ansi -I../src -o $@

STDCPP=stdc++ -lm
EXECUTABLE = -O2 -o crttest

crttest: crttest.o flintpp.o flint.o random.o entropy.o aes.o ripemd.o sha1.o randompp.o
	gcc $(EXECUTABLE) crttest.o random.o entropy.o aes.o ripemd.o sha1.o randompp.o flintpp.o flint.o -l$(STDCPP)

crttest.o: crttest.cpp
	gcc $(CFLAGS) crttest.cpp

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


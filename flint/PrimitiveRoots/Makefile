CFLAGS = -v -c -O2 -Wall -ansi -I../src -o $@

STDCPP=stdc++
EXECUTABLE = -ggdb -o primroot -lm -fpermissive

rsademo: primroot.o flintpp.o flint.o 
	gcc $(EXECUTABLE) primroot.o flintpp.o flint.o -l$(STDCPP) 

primroot.o: primroot.cpp
	gcc $(CFLAGS) primroot.cpp

flintpp.o: ../src/flintpp.cpp
	gcc $(CFLAGS) ../src/flintpp.cpp

flint.o: ../src/flint.c
	gcc $(CFLAGS) ../src/flint.c



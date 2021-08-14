#include <cstdlib>
#include <cstdio>
#include <time.h>

#include "../src/flintpp.h"

int main(int argc, char **argv)
{
	int primecount = 0;
    int x = atoi(argv[1]);

	printf("Printing up to 10 primitive roots of %s\n", argv[1]);

    for(int i = 1; i < x; ++i)
    {
		// keep track of remainders, so we can look for repeats
    	LINT remainders[x];
    	bool repeat = false;
        for(int j = 1; j < x; ++j)
        {
			// modular exponentiation here; i ^ j mod x
			LINT w = mexp(LINT(i), LINT(j), LINT(x));
            remainders[j - 1] = w;

			// since modular exponentiation is expensive, check for duplicates here, and bail
			// if we find any
			for(int k = 0; k < j - 1; ++k)
			{
				if(remainders[j - 1] == remainders[k]) 
				{
					repeat = true;
					break;
				}
			}
			if(repeat) break;
        }
		if(!repeat) 
		{
			printf("%i is a primitive root of %i\n", i, x);
			if(++primecount >= 10) break;
		}

    }

	return 0;
}


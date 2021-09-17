#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../src/flintpp.h"
#include "../src/random.h"



int InitRand (STATEPRNG& , const char*, int, int, int);
LINT FindPrime (unsigned short, STATEPRNG&);

// this just tracks elapsed time in seconds, to time various parts of the process
class Timekeeper
{
	time_t lastTime;

public:
	Timekeeper(void)
	{
		lastTime = time(NULL);
	}
	void Print(void)
	{
		time_t current = time(NULL);
		printf("Elapsed time %i seconds\n", int(current - lastTime));
		lastTime = current;
	}
};

int main(int argc, char **argv)
{
	// length of primes; half the RSA key length
	unsigned short bits = 1024; // no greater than 4097 due to flint max hex digits set at 512
	// pick a small odd prime as the public exponent e
	// ICC cards typically use 3, 65537 (2^16 + 1) is also common
	unsigned short e = 3;

	if(argc == 2) bits = atoi(argv[1]);
	
	printf("Calculating %i bit primes\n", bits);
	if(bits <= 16) 
	{
		printf("Bit length too small, must be greater than 16.\n");
		exit(0);
	}
	if(bits > 4096)
	{
		printf("Bit length too great, must be 4096 or less.\n");
		exit(0);
	}

	// LINT are created with the value and the base as arguments; we'll need 1 and 0 later
	LINT one("1", 16);
	LINT zero("0", 16);

	// p and q are random large primes, n is the modulus, t is Euler's totient, 
	//  d is the private exponent
	LINT p, q, n, t, d;
	Timekeeper tk;

	STATEPRNG prngState;
	int missingBits = InitRand(prngState, "", 0, 200, FLINT_RNDRMDSHA1);
	if(missingBits > 0)
	{
		printf("Warning:  Insufficient entropy for generating RSA keys, %i bits short.\n\n", missingBits);
	}
	else if(missingBits < 0)
	{
		printf("Warning:  Selected prng not available.\n\n");
		return -1;
	}
  
	printf("Calculating %i bit primes for %i bit keys, public exponent 0x%x.\n\n", bits, 2 * bits, e);
	// find two primes, p and q
	//  calculate primes whose p - 1 is not factorable by e; if either is, then the
	//  totient will be factorable by e, and the private exponent search won't work
	LINT p_1_mod_e;
	do
	{
		p = FindPrime(bits, prngState);
		printf("p 0x%s\n\n", p.hexstr());
		tk.Print();

		printf("Checking for factorability...\n");
		p_1_mod_e = mod(p - 1, e);
		tk.Print();

		if(p_1_mod_e == zero)
			printf("p - 1 is factorable by e, trying again.\n");
	}while(p_1_mod_e == zero);

	LINT q_1_mod_e;
	do
	{
		q = FindPrime(bits, prngState);
		printf("q 0x%s\n\n", q.hexstr());
		tk.Print();

		printf("Checking for factorability...\n");
		q_1_mod_e = mod(q - 1, e);
		tk.Print();

		if(q_1_mod_e == zero)
			printf("q - 1 is factorable by e, trying again.\n");
	}while(q_1_mod_e == zero);

	// CRT is iffy with q>p, so swap them if that's the case
	if(q > p)
	{
		LINT tmp = p;
		p = q;
		q = tmp;
	}
	
	// calculate modulus, n = p * q
	n = mul(p, q);
	printf("n 0x%s\n\n", n.hexstr());
	tk.Print();

	// calculate totient t = (p - 1)(q - 1)
	t = mul(sub(p, one), sub(q, one));
	printf("Calculated totient (p - 1)(q - 1)\n");
	tk.Print();

	// flint function to calculate modular inverse
	// d = inv(e, t);
	// Or, do it by hand:
	// find the modular multiplicative inverse of the public key exponent modulo the totient
	//  start with a k of 1
	LINT k("1", 16);
	int count = 0;
	while(true)
	{
		++count;
		k.add(t);	// add t each iteration, so k is never a multiple of t
		// calculate k modulus e to see if it's a multiple
		LINT remainder = mod(k, e);
		if(zero == remainder)
		{
			// k / e is the private exponent d
			d = divr(k, e, remainder);
			break;
		}
	}
	printf("Found private exponent after %i iterations\n\n", count);
	printf("d 0x%s\n\n", d.hexstr());
	tk.Print();

	// Test message.  The message is treated as a large number, so it needs to be encoded 
	//  as hex.  Converting ASCII to hex would work, or use a more compact encoding for 
	//  plain text.
	char text[bits / 2 + 1];
	for(count = 0; count < bits / 2; ++count)
	{
		int hex = count % 16;
		char digit[8];
		sprintf(digit, "%x", hex);
		text[count] = digit[0];
	}

	// message text must be no larger than the modulus, so truncate it one character short.  
	//  For a long message, break it into n bit blocks and  encode each block separately
	if(strlen(text) > strlen(n.hexstr()))
		text[strlen(n.hexstr()) - 2] = 0;

	{
		LINT message(text, 16);
		LINT cipher = mexp(message, e, n);
		printf("Ciphered (message ^ e mod n) 0x%s\n\n", cipher.hexstr());
		tk.Print();
	
		message = mexpkm(cipher, d, n);
		printf("Deciphered (cipher ^ d mod n) 0x%s\n\n", message.hexstr());
		tk.Print();

	// do the same operation using the CRT
	/* CRT explanation from http://www.di-mgt.com.au/crt_rsa.html
	
	We can use the CRT to compute m = cd mod n more efficiently. The full algorithm 
	from [PKCS1] is

    Precompute the following values given p, q with p > q

    dP = (1/e) mod (p-1)
    dQ = (1/e) mod (q-1)
    qInv = (1/q) mod p

    where the (1/e) notation means the modular inverse. The expression 
	x = (1/e) mod N is also written as x = e^-1 mod N, and x is any integer that 
	satisfies x*e ≡ 1 (mod N). In our case, where N = n = pq, we use the unique 
	value x in Zn, the set of numbers {0, 1, 2, ..., n-1}.

    To compute the message m given c do
    m1 = c^dP mod p
    m2 = c^dQ mod q
    h = qInv*(m1 - m2 + p) mod p  (Add an extra p to keep things positive.)
    m = m2 + h*q

	We store our private key as the quintuple (p, q, dP, dQ, qInv).
	
	As to why the extra p to keep things positive, see Applied Crytography, Bruche Schneier, section 11.3 pg 242-243:
	
	"This definition of mod may be different from the definition used in some programming languages.  For example, 
	PASCAL's modulo operator sometimes returns a negative number.  It returns a number between -(n - 1) and n - 1. 
	In C, the % operator returns the remainder from the division of the first expression by the second; this can be 
	a negative number if either operand is  negative.  For all the algorithms in this book, make sure you add n to 
	the result of the modulo operator if  it  returns a negative number."

	*/ 

		if(p > q)
		{
			LINT dP = inv(e, p - 1);
			printf("dP 0x%s\n\n", dP.hexstr());
			LINT dQ = inv(e, q - 1);
			printf("dQ 0x%s\n\n", dQ.hexstr());
			LINT qInv = inv(q, p);
			printf("qInv 0x%s\n\n", qInv.hexstr());

			LINT m1 = mexpkm(cipher, dP, p);
			printf("m1 0x%s\n\n", m1.hexstr());
			LINT m2 = mexpkm(cipher, dQ, q);
			printf("m2 0x%s\n\n", m2.hexstr());
			LINT h = qInv.mmul(m1 - m2 + p, p);	
			printf("h 0x%s\n\n", h.hexstr());
			message = m2 + h * q;

			printf("Deciphered using CRT 0x%s\n\n", message.hexstr());
			tk.Print();
		}
		else
		{
			printf("p must be greater than q to use the CRT, message not deciphered with CRT\n\n");
		}
	}

	printf("The public key is the pair (e, n).  This is used to verify data signed by the private key holder, "
		"or encrypt data to be sent to the private key holder.\n");

	printf("The private key is either the pair (e, d) (for standard RSA) or the set (e, p, q, dP, dQ, qInv) if using CRT.\n");

	return 0;
}


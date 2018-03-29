#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// FLINT bignum libraries, from "Cryptography in C and C++" by Michael Welschenbach, Apress
#include "../src/flintpp.h"
#include "../src/random.h"

/* 
CRT explanation from http://www.di-mgt.com.au/crt_rsa.html

We can use the CRT to compute m = c^d mod n more efficiently. The full algorithm 
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

This demonstrates why the common requirement that p > q exists; m1 is less
than m2, and so the modular multiplication to get h results in a negative
number.  To prevent this, p should be added to m1 - m2, so that the result
will always be positive.  Swap p and q values, and the problem goes away,
add p to keep the qInv operation postive, and the problem goes away.
*/ 

LINT ReadHexDigits(char *prompt)
{
	LINT number("0", 16);
	int count;
	char *str;
	printf("%s", prompt);
	count = scanf("%ms", &str);
	if(count > 0) 
	{
		number = LINT(str, 16);
		free(str);
	}
	else
	{
		printf("No value entered, exiting\n");
		return -1;
	}
	return number;
}

int main(int argc, char **argv)
{
	printf("This program accepts the public exponent e and basis primes p and q from an RSA\n"
	       "key set, and calculates the modulus n, private exponent d, and the Chinese\n"
	       "Remainder Theorem values dP, dQ, and qInv.  It then runs a test to ensure that\n"
	       "a signing operation with the public key components can be verified with the CRT\n"
	       "components.\n");

	LINT e = ReadHexDigits("Enter e: ");
	LINT p = ReadHexDigits("Enter p: ");
	LINT q = ReadHexDigits("Enter q: ");
	printf("\n");

	// calculate all the components from p and q
	LINT n = mul(p, q);
	LINT d = inv(e, mul(p - 1, q - 1));
	
	LINT dP = inv(e, p - 1);
	LINT dQ = inv(e, q - 1);
	LINT qInv = inv(q, p);

	printf("Calculated components:\n");
	printf("n:    %s\n", n.hexstr());
	printf("d:    %s\n", d.hexstr());
	printf("\n");
	printf("dP:   %s\n", dP.hexstr());
	printf("dQ:   %s\n", dQ.hexstr());
	printf("qInv: %s\n", qInv.hexstr());
	printf("\n");

	// create a hex message of the same length as the modulus
	int msglength = strlen(n.hexstr());
	printf("Key length %i bits\n", msglength * 4);

	char msgstring[2049] = "123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0";
	msgstring[msglength] = 0;

	LINT message(msgstring, 16);
	printf("Testing with message %s\n", message.hexstr());

	// sign with the public key components e and n
	LINT cipher = mexpkm(message, e, n);

	printf("Signed message %s\n", cipher.hexstr());

	// do a standard verification with d and n
	LINT decrypted = mexpkm(cipher, d, n);

	printf("Recovered message with d and n %s\n", decrypted.hexstr());

	if(message != decrypted) 
		printf("Private key failed!\n");
	else
		printf("Private key worked.\n");

	// do a CRT verification using p, q, dP, dQ, qInv
	LINT m1 = mexpkm(cipher, dP, p);
	LINT m2 = mexpkm(cipher, dQ, q);
	LINT h = qInv.mmul(p + m1 - m2, p);	// add an extra p to prevent a negative message value
	LINT decrypted2 = m2 + h * q;

	printf("Recovered message with CRT components %s\n", decrypted2.hexstr());

	if(message != decrypted2) 
		printf("CRT failed!\n");
	else
		printf("CRT worked.\n");

	return 0;
}


//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module testprrt.cpp     Revision: 10.07.2002                               */
//                                                                            */
//  Copyright (C) 1998-2005 by Michael Welschenbach                           */
//  Copyright (C) 1998-2005 by Springer-Verlag Berlin, Heidelberg             */
//  Copyright (C) 2001-2005 by Apress L.P., Berkeley, CA                      */
//  Copyright (C) 2002-2005 by Wydawnictwa MIKOM, Poland                      */
//  Copyright (C) 2002-2005 by PHEI, P.R.China                                */
//  Copyright (C) 2002-2005 by InfoBook, Korea                                */
//  Copyright (C) 2002-2005 by Triumph Publishing, Russia                     */
//                                                                            */
//  All Rights Reserved                                                       */
//                                                                            */
//  The software may be used for noncommercial purposes and may be altered,   */
//  as long as the following conditions are accepted without any              */
//  qualification:                                                            */
//                                                                            */
//  (1) All changes to the sources must be identified in such a way that the  */
//      changed software cannot be misinterpreted as the original software.   */
//                                                                            */
//  (2) The statements of copyright may not be removed or altered.            */
//                                                                            */
//  (3) The following DISCLAIMER is accepted:                                 */
//                                                                            */
//  DISCLAIMER:                                                               */
//                                                                            */
//  There is no warranty for the software contained in this distribution, to  */
//  the extent permitted by applicable law. The copyright holders provide the */
//  software `as is' without warranty of any kind, either expressed or        */
//  implied, including, but not limited to, the implied warranty of fitness   */
//  for a particular purpose. The entire risk as to the quality and           */
//  performance of the program is with you.                                   */
//                                                                            */
//  In no event unless required by applicable law or agreed to in writing     */
//  will the copyright holders, or any of the individual authors named in     */
//  the source files, be liable to you for damages, including any general,    */
//  special, incidental or consequential damages arising out of any use of    */
//  the software or out of inability to use the software (including but not   */
//  limited to any financial losses, loss of data or data being rendered      */
//  inaccurate or losses sustained by you or by third parties as a result of  */
//  a failure of the software to operate with any other programs), even if    */
//  such holder or other party has been advised of the possibility of such    */
//  damages.                                                                  */
//                                                                            */
//*****************************************************************************/

#include <stdlib.h>
#include "flintpp.h"


#define MAXTESTLEN (CLINTMAXBIT/2)
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)

static int simple_tests (void);
static int testprimroot (int);

// Some primitive roots modulo p
//
//   p = 3:   2
//   p = 5:   2,3
//   p = 7:   3,5
//   p = 11:  2,6,7,8
//   p = 13:  2,6,7,11
//   p = 17:  3,5,6,7,10,11,12,14
//   p = 19:  2,3,10,13,14,15
//   p = 23:  5,7,10,11,14,15,17,19,20,21
//   p = 29:  2,3,8,10,11,14,15,118,19,21,26,27
//   p = 31:  3,11,12,13,17,21,22,24
//   p = 37:  2,5,13,15,17,18,19,20,22,24,32,35
//   p = 41:  6,7,11,12,13,15,17,19,22,24,26,28,29,30,34,35
//   p = 43:  3,5,12,18,19,20,26,28,29,30,33,34
//   p = 47:  5,10,11,13,15,19,20,22,23,26,29,30,31,33,35,38,39,40,41,43,44,45


static int testdata[130][7] = {

// Data structure: p, r, n, p1,...,pn
// p = Primenumber modulus
// r = Smallest primitive root modulo p (cf. above table)
// n = Number of prime divisors of the group order p - 1 in Z/pZ
// p1,...,pn: Prime divisors of the group order p - 1

  { 3, 2, 1, 2},
  { 5, 2, 1, 2},
  { 7, 3, 2, 2 , 3},
  {11, 2, 2, 2,  5},
  {13, 2, 2, 2,  3},
  {17, 3, 1, 2},
  {19, 2, 2, 2,  3},
  {23, 5, 2, 2, 11},
  {29, 2, 2, 2,  7},
  {31, 3, 3, 2,  3, 5},
  {37, 2, 2, 2,  3},
  {41, 6, 2, 2,  5},
  {43, 3, 3, 2,  3, 7},
  {47, 5, 2, 2, 23},
  {53, 2, 2, 2, 13},
  {59, 2, 2, 2, 29},
  {61, 2, 3, 2, 3, 5},
  {67, 2, 3, 2, 3, 11},
  {71, 7, 3, 2, 5, 7},
  {73, 5, 2, 2, 3},
  {79, 3, 3, 2, 3, 13},
  {83, 2, 2, 2, 41},
  {89, 3, 2, 2, 11},
  {97, 5, 2, 2, 3},
  {101, 2, 2, 2, 5},
  {103, 5, 3, 2, 3, 17},
  {107, 2, 2, 2, 53},
  {109, 6, 2, 2, 3},
  {113, 3, 2, 2, 7},
  {127, 3, 3, 2, 3, 7},
  {131, 2, 3, 2, 5, 13},
  {137, 3, 2, 2, 17},
  {139, 2, 3, 2, 3, 23},
  {149, 2, 2, 2, 39},
  {151, 6, 3, 2, 3, 5},
  {157, 5, 3, 2, 3, 13},
  {163, 2, 2, 2, 3},
  {167, 5, 2, 2, 83},
  {173, 2, 2, 2, 43},
  {179, 2, 2, 2, 89},
  {181, 2, 3, 2, 3, 5},
  {191, 19, 3, 2, 5, 19},
  {193, 5, 2, 2, 3},
  {197, 2, 2, 2, 7},
  {199, 3, 3, 2, 3, 11},
  {211, 2, 4, 2, 3, 5, 7},
  {223, 3, 3, 2, 3, 37},
  {227, 2, 2, 2, 113},
  {229, 6, 3, 2, 3, 19},
  {233, 3, 2, 2, 29},
  {239, 7, 3, 2, 7, 17},
  {241, 7, 3, 2, 3, 5},
  {251, 6, 2, 2, 5},
  {257, 3, 1, 2},
  {263, 5, 2, 2, 131},
  {269, 2, 2, 2, 67},
  {271, 6, 3, 2, 3, 5},
  {277, 5, 3, 2, 3, 23},
  {281, 3, 3, 2, 5, 7},
  {283, 3, 3, 2, 3, 47},
  {293, 2, 2, 2, 73},
  {307, 5, 3, 2, 3, 17},
  {311, 17, 3, 2, 5, 31},
  {313, 10, 3, 2, 3, 13},
  {317, 2, 2, 2, 79},
  {331, 3, 4, 2, 3, 5, 11},
  {337, 10, 3, 2, 3, 7},
  {347, 2, 2, 2, 173},
  {349, 2, 3, 2, 3, 29},
  {353, 3, 2, 2, 11},
  {359, 7, 2, 2, 179},
  {367, 6, 3, 2, 3, 61},
  {373, 2, 3, 2, 3, 31},
  {379, 2, 3, 2, 3, 7},
  {383, 5, 2, 2, 191},
  {389, 2, 2, 2, 97},
  {397, 5, 3, 2, 3, 11},
  {401, 3, 2, 2, 5},
  {409, 21, 3, 2, 3, 17},
  {419, 2, 3, 2, 11, 19},
  {421, 2, 4, 2, 3, 5, 7},
  {431, 7, 3, 2, 5, 43},
  {433, 5, 2, 2, 3},
  {439, 15, 3, 2, 3, 73},
  {443, 2, 3, 2, 13, 17},
  {449, 3, 2, 2, 7},
  {457, 13, 3, 2, 3, 19},
  {461, 2, 3, 2, 5, 23},
  {463, 3, 4, 2, 3, 7, 11},
  {467, 2, 2, 2, 233},
  {479, 13, 2, 2, 239},
  {487, 3, 2, 2, 3},
  {491, 2, 3, 2, 5, 7},
  {499, 7, 3, 2, 3, 83},
  {503, 5, 2, 2, 251},
  {509, 2, 2, 2, 127},
  {521, 3, 3, 2, 5, 13},
  {523, 2, 2, 2, 29},
  {541, 2, 3, 2, 3, 5},
  {547, 2, 3, 2, 3, 7, 13},
  {557, 2, 2, 2, 139},
  {563, 2, 2, 2, 281},
  {569, 3, 2, 2, 71},
  {571, 3, 4, 2, 3, 5, 19},
  {577, 5, 2, 2, 3},
  {587, 2, 2, 2, 293},
  {593, 3, 2, 2, 37},
  {599, 7, 3, 2, 13, 23},
  {601, 7, 3, 2, 3, 5},
  {613, 2, 3, 2, 3, 17},
  {617, 3, 3, 2, 7, 11},
  {619, 2, 3, 2, 3, 103},
  {631, 3, 3, 2, 3, 5, 7},
  {641, 3, 2, 2, 5},
  {643, 11, 3, 2, 3, 107},
  {647, 5, 3, 2, 17, 19},
  {653, 2, 2, 2, 163},
  {659, 2, 3, 2, 7, 47},
  {661, 2, 4, 2, 3, 5, 11},
  {673, 5, 3, 2, 3, 7},
  {677, 2, 2, 2, 13},
  {683, 5, 3, 2, 11, 31},
  {691, 3, 4, 2, 3, 5, 23},
  {701, 2, 3, 2, 5, 7},
  {709, 2, 3, 2, 3, 59},
  {719, 11, 2, 2, 359},
  {727, 5, 3, 2, 3, 11},
  {733, 6, 3, 2, 3, 61},
  {739, 3, 3, 2, 3, 41},
  {743, 5, 3, 2, 7, 53}
};

// Parameter positions in testdata
#define MODULUS  0  // Primenumber modulus p in testdata[i][0]
#define PRIMROOT 1  // Smallest primitive root mod p in testdata[i][1]
#define NOOFFACT 2  // Number of prime divisors of p - 1 in testdata[i][4]

int main ()
{
  cout << "Testmodule " << __FILE__
       << " compiled for FLINT/C-Library Version "
       << verstr_l () << endl;

  initrand64_lt ();
  simple_tests ();
  testprimroot (100);

  cout << "All tests in " << __FILE__ << " passed." << endl;
  return 0;
}


static int simple_tests (void)
{
  LINT a;
  int test, j;

  cout << "Testfunction simple_tests() ..." << endl;

  for (test = 0; test < 130; test++)
    {
      LINT **primes = new LINT * [testdata[test][NOOFFACT] + 1];

      // Set primes[0] to p - 1
      primes[0] = new LINT (testdata[test][MODULUS] - 1);

      for (j = 1; j <= testdata[test][NOOFFACT]; j++)
        {
          // Set primes[j] to prime divisor of p - 1
          primes[j] = new LINT (testdata[test][NOOFFACT+j]);
        }

      // Calculate a = primitive root mod p
      a = primroot (testdata[test][NOOFFACT], primes);

      if (a < 0)
        {
          cerr << "Error in primroot near line " << __LINE__ << "!" << endl;
          cerr << "Returncode < 0 in test " << test << " ." << endl;
          exit (-1);
        }

      // Calculate u = a^(p - 1) and v = a^p
      LINT u = mexpkm (a, *primes[0] - 1, *primes[0] + 1);
      LINT v = mexpkm (a, *primes[0], *primes[0] + 1);

      if (!(u != 1 && v == 1))
        {
          cerr << "Error in primroot() near line " << __LINE__ << "!" << endl;
          cerr << "Unexpected value for primitive root in test " << test << ":" << endl;
          cerr << "Calculated primitive root: " << LintDec << a << endl;
          cerr << "Expected value: " << LINT (testdata[test][PRIMROOT]) << endl;
          exit (-1);
        }

      if (a != LINT (testdata[test][PRIMROOT]))
        {
          cerr << "Error in primroot() near line " << __LINE__ << "!" << endl;
          cerr << "Unexpected value for primitive root in test " << test << ":" << endl;
          cerr << "Calculated primitive root: " << LintDec << a << endl;
          cerr << "Expected value: " << LINT (testdata[test][PRIMROOT]) << endl;
          exit (-1);
        }

      for (j = 0; j <= testdata[test][NOOFFACT]; j++)
        {
          delete primes[j];
        }

      delete [] primes;
    }

  return 0;
}


static int testprimroot (int no_of_tests)
{
  LINT p, q, u, a, t;
  LINT **primes = new LINT *[10];
  USHORT sp;
  int i, j;

  cout << "Testfunction testprimroot() ..." << endl;

  initrand64_lt ();

  for (i = 1; i <= no_of_tests; i++)
    {
      p = randl (CLINTRNDLN % 256);
      p = nextprime (p, 1);

      u = randl (CLINTRNDLN % 256);
      u = nextprime (u, 1);

      t = p * u << 1;

      sp = 2;
      j = 1;

      do
        {
          sp += smallprimes[j++];
          q = t * sp + 1;
        }
      while (!isprime (q));

      // Now q = smallprime * u * p + 1
      // => q - 1 = smallprime * u * p

      q--;
      primes[0] = &q;
      primes[1] = &p;
      primes[2] = &u;
      LINT two (2);
      primes[3] = &two;
      LINT smallprime (sp);
      primes[4] = &smallprime;

      a = primroot (4, primes);

      if (a < 0)
        {
          cerr << "Error in primroot()!";
          exit (-1);
        }

      u = mexpkm (a, q - 1, q + 1);
      t = mexpkm (a, q, q + 1);

      if (!(u != 1 && t == 1))
        {
          cerr << "Error in primroot() near line " << __LINE__ << "!" << endl;
          cerr << "Calculated primitive root: " << LintHex << a << endl;
          exit (-1);
        }
    }

  delete [] primes;
  return 0;
}


//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module testroot.cpp     Revision: 10.07.2002                               */
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

#define PRIMERESTMOD4 3
#define GENERATOR FLINT_RNDRMDSHA1

#define MAXTESTLEN 64
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN - 1)



static int simple_tests (int);
static int general_tests (int);
static int general_large_tests (int);
static int test_iroot (int);

STATEPRNG xrstate;

main ()
{
  cout << "Testmodule " << __FILE__
       << " compiled for FLINT/C-Library Version "
       << verstr_l () << endl;

  initrand64_lt ();
  InitRand (xrstate, "", 0, 200, GENERATOR);

  simple_tests (1000);
  general_tests (50);
  general_large_tests (5);
  test_iroot (1000);

  cout << "All tests in " << __FILE__ << " passed." << endl;
  return 0;
}


static int simple_tests (int no_of_tests)
{
  LINT a, p, q, r;
  int jp, jq;

  cout << "Testfunction simple_tests() with constants ..." << endl;

  if (jacobi (0, 3) != 0)
    {
      cerr << "Error: jacobi(0, 3) != 0" << endl;
      exit (-1);
    }
  if (jacobi (3, 0) != 0)
    {
      cerr << "Error: jacobi(3, 0) != 0" << endl;
      exit (-1);
    }
  if (jacobi (8, 2) != 0)
    {
      cerr << "Error: jacobi(8, 2) != 0" << endl;
      exit (-1);
    }

  a = RandLINT (CLINTRNDLN, xrstate);
  r = root (a, 0);
  if (r.Get_Warning_Status() != E_LINT_ERR)
    {
      cerr << "Error: root (a, 0) != -1" << endl;
      exit (-1);
    }

  r = root (a, 2);
  if (r.Get_Warning_Status() != E_LINT_ERR)
    {
      cerr << "Error: root (a, 2) != -1" << endl;
      exit (-1);
    }

  r = a.root (0);
  if (r.Get_Warning_Status() != E_LINT_ERR)
    {
      cerr << "Error: a.root (0) != -1" << endl;
      exit (-1);
    }

  r = a.root (2);
  if (r.Get_Warning_Status() != E_LINT_ERR)
    {
      cerr << "Error: a.root (2) != -1" << endl;
      exit (-1);
    }

  cout << "Testfunction simple_tests() with small parameters ..." << endl;

  for (int i = 0; i < no_of_tests; i++)
    {
      p = FindPrime (MAXTESTLEN/2, xrstate);
      do
        {
          q = FindPrime (MAXTESTLEN/2, xrstate);
        }
      while (q % 4 == p % 4);

      // cout << "p = " << p << endl;
      // cout << "p mod 4 = " << p % 4 << endl;

      // cout << "q = " << q << endl;
      // cout << "q mod 4 = " << q % 4 << endl;

      do
        {
          a = RandLINT (CLINTRNDLN, xrstate);
          jp = a.jacobi (p);
          r = mexp (a, ( p - 1 ) / 2, p);
          if (jp < 0) r += -jp;
          if (jp > 0) r -= jp;
          if (r % p > 0)
            {
              cerr << "Error: a^(p-1)/2 = " << r << " != (a/p) mod p" << endl;
              exit (-1);
            }

          jq = a.jacobi (q);
          r = mexp (a, ( q - 1 ) / 2, q);
          if (jq < 0) r += -jq;
          if (jq > 0) r -= jq;
          if (r % q > 0)
            {
              cerr << "Error: a^(q-1)/2 = " << r << " != (a/q) mod q" << endl;
              exit (-1);
            }
        } while (( a > p * q ) || (( a.jacobi (p) < 1 ) || ( a.jacobi (q) < 1 )));

      // cout << "a = " << a << endl;
      // cout << "jacobi(a,p) = " << jacobi(a,p) << ", ";
      // cout << "jacobi(a,q) = " << jacobi(a,q) << endl;

      if (( r = root (a, p, q)) == 0U)
        {
          cerr << "Error in jacobi: root = NULL " << endl;
          cerr << "a is not a quadratic residue mod pq." << endl;
          cerr << "a = " << a << endl;
          cerr << "jacobi(a,p) = " << jacobi (a, p) << ", ";
          cerr << "jacobi(a,q) = " << jacobi (a, q) << endl;
          exit (-1);
        }

      // cout << "Root of a mod pq = " << r << endl;
      if (a == ( r * r ) % ( p * q ))
        {
          // cout << "a is quadratic residue mod pq." << endl;
        }
      else
        {
          cerr << "Error: a is not a quadratic residue mod pq." << endl;
          cerr << "a = " << a << endl;
          cerr << "jacobi(a,p) = " << jacobi (a, p) << ", ";
          cerr << "jacobi(a,q) = " << jacobi (a, q) << endl;
          cerr << "r = " << r << endl;
          exit (-1);
        }
    }

  return 0;
}


#undef  MAXTESTLEN
#define MAXTESTLEN (CLINTMAXBIT >> 2)

static int general_tests (int no_of_tests)
{
  LINT a, p, q, r;
  int jp, jq;

  cout << "Testfunction general_tests() ..." << endl;

  for (int i = 0; i < no_of_tests; i++)
    {
      p = FindPrime (MAXTESTLEN/2, xrstate);
      do
        {
          q = FindPrime (MAXTESTLEN/2, xrstate);
        } while (p % 4 == q % 4);

      // cout << "p = " << p << endl;
      // cout << "p mod 4 = " << p % 4 << endl;

      // cout << "q = " << q << endl;
      // cout << "q mod 4 = " << q % 4 << endl;

      do
        {
	  a = RandLINT (CLINTRNDLN, xrstate);
          jp = a.jacobi (p);
          r = mexp (a, ( p - 1 ) / 2, p);
          if (jp < 0) r += -jp;
          if (jp > 0) r -= jp;
          if (r % p > 0)
            {
              cerr << "Error: a^(p-1)/2 = " << r << " != (a/p) mod p" << endl;
              exit (-1);
            }

          jq = a.jacobi (q);
          r = mexp (a, ( q - 1 ) / 2, q);
          if (jq < 0) r += -jq;
          if (jq > 0) r -= jq;
          if (r % q > 0)
            {
              cerr << "Error: a^(q-1)/2 = " << r << " != (a/q) mod q" << endl;
              exit (-1);
            }
        } while (( a > p * q ) || ( a.jacobi (p) < 1 ) || ( a.jacobi (q) < 1 ));

      // cout << "a = " << a << endl;
      // cout << "jacobi(a,p) = " << jacobi(a,p) << ", ";
      // cout << "jacobi(a,q) = " << jacobi(a,q) << endl;

      if (( r = a.root (p, q)) == 0U)
        {
          cerr << "Error in jacobi: root = NULL " << endl;
          cerr << "a is not a quadratic residue mod pq." << endl;
          cerr << "a = " << a << endl;
          cerr << "jacobi(a,p) = " << jacobi (a, p) << ", ";
          cerr << "jacobi(a,q) = " << jacobi (a, q) << endl;
          exit (-1);
        }

      // cout << "Root of a mod pq = " << r << endl;
      if (a == ( r * r ) % ( p * q ))
        {
        // cout << "a is a quadratic residue mod pq." << endl;
        }
      else
        {
          cerr << "Error: a is not a quadratic residue mod pq." << endl;
          cerr << "a = " << a << endl;
          cerr << "jacobi(a,p) = " << jacobi (a, p) << ", ";
          cerr << "jacobi(a,q) = " << jacobi (a, q) << endl;
          cerr << "r = " << r << endl;
          exit (-1);
        }
    }

  return 0;
}


#undef  MAXTESTLEN
#define MAXTESTLEN (CLINTMAXBIT >> 1)

static int general_large_tests (int no_of_tests)
{
  LINT a, p, q, r;
  int jp, jq;

  cout << "Testfunction general_large_tests() ..." << endl;

  for (int i = 0; i < no_of_tests; i++)
    {
      p = FindPrime (MAXTESTLEN/2, xrstate);
      do
        {
          q = FindPrime (MAXTESTLEN/2, xrstate);
        } while (p % 4 == q % 4);

      // cout << "p = " << p << endl;
      // cout << "p mod 4 = " << p % 4 << endl;

      // cout << "q = " << q << endl;
      // cout << "q mod 4 = " << q % 4 << endl;

      do
        {
	  a = RandLINT (CLINTRNDLN, xrstate);
          jp = a.jacobi (p);
          r = mexp (a, ( p - 1 ) / 2, p);
          if (jp < 0) r += -jp;
          if (jp > 0) r -= jp;
          if (r % p > 0)
            {
              cerr << "Error: a^(p-1)/2 = " << r << " != (a/p) mod p" << endl;
              exit (-1);
            }

          jq = a.jacobi (q);
          r = mexp (a, ( q - 1 ) / 2, q);
          if (jq < 0) r += -jq;
          if (jq > 0) r -= jq;
          if (r % q > 0)
            {
              cerr << "Error: a^(q-1)/2 = " << r << " != (a/q) mod q" << endl;
              exit (-1);
            }
        } while (( a > p * q ) || ( a.jacobi (p) < 1 ) || ( a.jacobi (q) < 1 ));

      // cout << "a = " << a << endl;
      // cout << "jacobi(a,p) = " << jacobi(a,p) << ", ";
      // cout << "jacobi(a,q) = " << jacobi(a,q) << endl;

      if (( r = a.root (p, q)) == 0U)
        {
          cerr << "Error in jacobi: root = NULL " << endl;
          cerr << "a is not a quadratic residue mod pq." << endl;
          cerr << "a = " << a << endl;
          cerr << "jacobi(a,p) = " << jacobi (a, p) << ", ";
          cerr << "jacobi(a,q) = " << jacobi (a, q) << endl;
          exit (-1);
        }

      // cout << "Root of a mod pq = " << r << endl;
      if (a == r.msqr (p * q))
        {
        // cout << "a is a quadratic residue mod pq." << endl;
        }
      else
        {
          cerr << "Error: a is not a quadratic residue mod pq." << endl;
          cerr << "a = " << a << endl;
          cerr << "jacobi(a,p) = " << jacobi (a, p) << ", ";
          cerr << "jacobi(a,q) = " << jacobi (a, q) << endl;
          cerr << "r = " << r << endl;
          exit (-1);
        }
    }

  return 0;
}


#undef  MAXTESTLEN
#define MAXTESTLEN CLINTMAXBIT

static int test_iroot (int nooftests)
{
  LINT n, r, q;
  int i;

  cout << "Test of functions issqr() for the detection of squares " << endl;
  cout << "and root() for the calculation of integer square roots..." << endl;

  n = LINT(0);
  q = n * n;
  r = q.issqr();

  if (r != n)
    {
      cerr << "Error: Root false in test " << 0 << " near line " << __LINE__ << endl;
      cerr << "n = " << n << endl << "r = " << r << endl;
      exit (-1);
    }

  if (q.root() != n)
    {
      cerr << "Error: Root false in test " << 0 << " near line " << __LINE__ << endl;
      cerr << "n = " << n << endl << "r = " << r << endl;
      exit (-1);
    }

  for (i = 1; i <= nooftests; i++)
    {
      n = RandLINT (CLINTRNDLN/2, xrstate);
      q = n * n;
      r = q.issqr();

      if ((n > 0) && (0 == r))
        {
          cerr << "Error: Square not detected in test " << i << " near line " << __LINE__ << endl;
          cerr << "n = " << n << endl << "r = " << r << endl;
          exit (-1);
        }

      if (r != n)
        {
          cerr << "Error: Root false in test " << i << " near line " << __LINE__ << endl;
          cerr << "n = " << n << endl << "r = " << r << endl;
          exit (-1);
        }

      if (q.root() != n)
        {
          cerr << "Error: Root false in test " << i << " near line " << __LINE__ << endl;
          cerr << "n = " << n << endl << "r = " << r << endl;
          exit (-1);
        }
    }


  n = LINT(0);
  q = n * n;
  r = issqr(q);

  if (r != n)
    {
      cerr << "Error: Root false in test " << 0 << " near line " << __LINE__ << endl;
      cerr << "n = " << n << endl << "r = " << r << endl;
      exit (-1);
    }

  if (root(q) != n)
    {
      cerr << "Error: Root false in test " << 0 << " near line " << __LINE__ << endl;
      cerr << "n = " << n << endl << "r = " << r << endl;
      exit (-1);
    }

  for (i = 1; i <= nooftests; i++)
    {
      n = RandLINT (CLINTRNDLN/2, xrstate);
      q = n * n;
      r = issqr(q);

      if ((n > 0) && (0 == r))
        {
          cerr << "Error: Square not detected in test " << i << " near line " << __LINE__ << endl;
          cerr << "n = " << n << endl << "r = " << r << endl;
          exit (-1);
        }

      if (r != n)
        {
          cerr << "Error: Root false in test " << i << " near line " << __LINE__ << endl;
          cerr << "n = " << n << endl << "r = " << r << endl;
          exit (-1);
        }

      if (root(q) != n)
        {
          cerr << "Error: Root false in test " << i << " near line " << __LINE__ << endl;
          cerr << "n = " << n << endl << "r = " << r << endl;
          exit (-1);
        }
    }

  return 0;
}




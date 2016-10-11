//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module testari.cpp      Revision: 10.07.2002                               */
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

static void report_error (LINT&, LINT&, LINT&, LINT&, int);
static void report_error (LINT&, LINT&, LINT&, int);
static void report_error (LINT&, LINT&, int);
static void testadd (int);
static void testsub (int);
static void testmul (int);
static void testsqr (int);
static void testdivr (int);
static void testmequ (int);
static void testmexp (int);
static void testfswap (int);
static int testset (int);
static int testshift (int);
static int testmax (void);

#define MAXTESTLEN CLINTMAXBIT/2
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)

#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

int main ()
{
  cout << "Testmodule " << __FILE__
       << " compiled for FLINT/C-Library Version "
       << verstr_l () << endl;

  initrand64_lt ();
  testadd (1000);
  testsub (1000);
  testmul (1000);
  testsqr (1000);
  testdivr (1000);
  testmequ (1000);
  testmexp (10);
  testfswap (1000);
  testset (1000);
  testshift (1000);
  testmax ();

  cout << "All tests in " << __FILE__ << " passed." << endl;
  return 0;
}


static void testadd (int no_of_tests)
{
  LINT a, b, c, d, e, m;
  int i;

  cout << "Testfunction testadd() ..." << endl;

  for (i = 1; i < no_of_tests; i++)
    {
      c = a = randl (CLINTRNDLN);
      d = b = randl (CLINTRNDLN);
      if ((( e = ( a + b )) != ( b + a )) ||
                   ( e != c.add (b))      ||
                   ( e != d.add (a))      ||
                   ( e != add (a, b))     ||
                   ( e != add (b, a)))
          report_error (a, b, c, d, __LINE__);

      c = a = randl (CLINTRNDLN);
      d = b = randl (CLINTRNDLN);
      if (( a += b ) != ( c += d ))
          report_error (a, b, c, d, __LINE__);

      c = a = randl (CLINTRNDLN);
      d = b = randl (CLINTRNDLN);
      do
        {
          m = randl (CLINTRNDLN);
        } while (0 == m);

      if ((( e = (( a + b ) % m )) != ( b + a ) % m ) ||
                   ( e != c.madd (b, m))              ||
                   ( e != d.madd (a, m))              ||
                   ( e != madd (a, b, m))             ||
                   ( e != madd (b, a, m)))
          report_error (a, b, c, d, __LINE__);
    }
}


static void testsub (int no_of_tests)
{
  LINT a, b, c, d, e, m;
  int i;

  cout << "Testfunction testsub() ..." << endl;

  for (i = 1; i < no_of_tests; i++)
    {
      c = a = randl (CLINTRNDLN);
      d = b = randl (CLINTRNDLN);
      if ((( e = ( a - b )) != 0 - ( b - a )) ||
                   ( e != c.sub (b))          ||
                   ( e != sub (a, b)))
        {
          cerr << "a - b = " << a - b;
          cerr << "0 - (b - a) = " << 0 - ( b - a );
          cerr << "sub(a,b) = " << sub (a, b);
          cerr << "a.sub(b) = " << a.sub (b);
          report_error (a, b, c, d, __LINE__);

          c = a = randl (CLINTRNDLN);
          d = b = randl (CLINTRNDLN);
          if (a < b)
            {
              fswap (a, b);
              fswap (c, d);
            }

          if (( a -= b ) != ( c -= d ))
              report_error (a, b, c, d, __LINE__);

          c = ( a = randl (CLINTRNDLN));
          d = ( b = randl (CLINTRNDLN));
          do
            {
              m = randl (CLINTRNDLN);
            } while (0 == m);

          if (c.msub (b, m) != msub (a, b, m))
              report_error (a, b, c, d, __LINE__);
        }
    }
}


static void testmul (int no_of_tests)
{
  LINT a, b, c, d, e, m;
  int i;

  cout << "Testfunction testmul() ..." << endl;

  for (i = 1; i < no_of_tests; i++)
    {
      c = a = randl (CLINTRNDLN);
      d = b = randl (CLINTRNDLN);
      if ((( e = ( a * b )) != ( b * a )) ||
                   ( e != c.mul (b))      ||
                   ( e != d.mul (a))      ||
                   ( e != mul (a, b))     ||
                   ( e != mul (b, a)))
          report_error (a, b, c, d, __LINE__);

      c = a = randl (CLINTRNDLN);
      d = b = randl (CLINTRNDLN);
      if (( a *= b ) != ( c *= d ))
          report_error (a, b, c, d, __LINE__);

      c = a = randl (CLINTRNDLN);
      d = b = randl (CLINTRNDLN);
      do
        {
          m = randl (CLINTRNDLN);
        } while (0 == m);

      if ((( e = (( a * b ) % m )) != ( b * a ) % m ) ||
                   ( e != c.mmul (b, m))              ||
                   ( e != d.mmul (a, m))              ||
                   ( e != mmul (a, b, m))             ||
                   ( e != mmul (b, a, m)))
          report_error (a, b, c, d, __LINE__);
    }
}


static void testsqr (int no_of_tests)
{
  LINT a, b, c, d, e, m;
  int i;

  cout << "Testfunction testsqr() ..." << endl;

  for (i = 1; i < no_of_tests; i++)
    {
      a = randl (CLINTRNDLN);
      b = a;
      if (( b * b ) != a.sqr ())
          report_error (a, b, c, d, __LINE__);

      a = randl (CLINTRNDLN);
      b = a;
      do
        {
          m = randl (CLINTRNDLN);
        } while (0 == m);

      if (( b = ( msqr (b, m))) != a.msqr (m))
          report_error (a, b, c, d, __LINE__);
    }
}


static void testdivr (int no_of_tests)
{
  LINT a, b, c, d, e, r, r1, r2;
  int i;

  cout << "Testfunction testdivr() ..." << endl;

  for (i = 1; i < no_of_tests; i++)
    {
      c = a = randl (CLINTRNDLN);
      do
        {
          d = b = randl (CLINTRNDLN);
        } while (0 == b);

      if ((( e = ( a / b )) != divr (a, b, r)) || ( e != c.divr (b, r)))
          report_error (a, b, c, d, __LINE__);

      c = a = randl (CLINTRNDLN);
      do
        {
          d = b = randl (CLINTRNDLN);
        } while (0 == b);
      if (( a /= b ) != ( c /= d ))
          report_error (a, b, c, d, __LINE__);

      c = a = randl (CLINTRNDLN);
      do
        {
          d = b = randl (CLINTRNDLN);
        } while (0 == b);

      divr (a, b, r1);
      a.divr (b, r2);
      if ((( e = ( c % d )) != r1 ) || ( e != r2 ))
          report_error (a, b, c, d, __LINE__);

      c = a = randl (CLINTRNDLN);
      do
        {
          d = b = randl (CLINTRNDLN);
        } while (0 == b);
      if (( a %= b ) != ( c %= d ))
          report_error (a, b, c, d, __LINE__);
    }
}


static void testmequ (int no_of_tests)
{
  LINT a, b, k, m;
  int i;

  cout << "Testfunction testmequ() ..." << endl;

  for (i = 1; i < no_of_tests; i++)
    {
      a = randl (CLINTRNDLN);
      b = randl (CLINTRNDLN);
      do
        {
          m = randl (CLINTRNDLN);
        }
      while (m == 0);

      if ((mequ (a, b, m) && !(a % m == b % m)) || (!mequ (a, b, m) && (a % m == b % m)))
        report_error (a, b, m, __LINE__);

      if ((a.mequ (b, m) && !(a % m == b % m)) || (!a.mequ (b, m) && (a % m == b % m)))
        report_error (a, b, m, __LINE__);
    }

  for (i = 1; i < no_of_tests; i++)
    {

      b = randl (CLINTRNDLN);
      do
        {
          k = root (randl (CLINTRNDLN));
          m = root (randl (CLINTRNDLN));
        }
      while (m == 0 || k == 0);

      a = k * m + b;

      if ((mequ (a, b, m) && !(a % m == b % m)) || (!mequ (a, b, m) && (a % m == b % m)))
        report_error (a, b, m, __LINE__);

      if ((a.mequ (b, m) && !(a % m == b % m)) || (!a.mequ (b, m) && (a % m == b % m)))
        report_error (a, b, m, __LINE__);
    }
}


static void testmexp (int no_of_tests)
{
  LINT a, b, c, d, e, f, g, m;
  USHORT u;
  int i;

  cout << "Testfunction testmexp() ..." << endl;

  for (i = 1; i < no_of_tests; i++)
    {
      c = a = randl (CLINTRNDLN);
      e = b = randl (CLINTRNDLN);
      m = randl (CLINTRNDLN) | 1;

      if ((( d = mexp (a, b, m)) != mexp5m (a, b, m)) || ( d != mexpkm (a, b, m)))
          report_error (a, b, m, d, __LINE__);


      f = g = c = a = randl (CLINTRNDLN);
      b = randl (CLINTRNDLN);
      m = randl (CLINTRNDLN) | 1;

      if ((( d = a.mexp (b, m)) != c.mexp (b, m)) ||
                          (d != f.mexp5m (b, m)) ||
                          (d != g.mexpkm (b, m)))
          report_error (a, b, m, d, __LINE__);

      c = a = randl (CLINTRNDLN);
      e = u = usrand64_l();
      if (a.mexp (u, m) != (d  = mexp (c, e, m)) ||
                           (d != mexp (c, u, m)))
          report_error (a, b, m, d, __LINE__);

      a = randl (CLINTRNDLN);
      e = u = usrand64_l();
      if (mexp (u, a, m) != (d = mexp (e, a, m)))
          report_error (a, e, m, d, __LINE__);

      c = a = randl (CLINTRNDLN);
      e = u = usrand64_l () % 512;
      if (a.mexp2 (u, m) != (d = mexp2 (c, u, m)))
          report_error (a, e, m, d, __LINE__);
    }
}


static void testfswap (int no_of_tests)
{
  LINT a, b, c, d;
  int i;

  cout << "Testfunction testfswap() ..." << endl;

  for (i = 1; i < no_of_tests; i++)
    {
      a = c = randl (CLINTRNDLN);
      b = d = randl (CLINTRNDLN);
      a.fswap (b);
      if (( a != d ) || ( b != c ))
          report_error (a, b, d, c, __LINE__);
      fswap (a, b);
      if (( a != c ) || ( b != d ))
          report_error (a, b, c, d, __LINE__);
    }
}


static int testset (int no_of_tests)
{
  LINT s = 0, t;
  unsigned long pos;

  cout << "Testfunction testset() ..." << endl;

  for (int i = 0; i < no_of_tests; i++)
    {
      s = randl (ulrand64_l () % CLINTMAXBIT);
      pos = ulrand64_l () % CLINTMAXBIT;
      t = s;
      s.setbit (pos);

      if (!s.testbit (pos))
        {
          cerr << "Error in setbit, testbit:" << endl << s << endl << "Pos = " << pos << endl;
          exit (1);
        }
      s.clearbit (pos);

      if (s.testbit (pos))
        {
          cerr << "Error in clearbit, testbit:" << endl << s << endl << "Pos = " << pos << endl;
          exit (1);
        }

      if (t.testbit (pos))
          t.clearbit (pos);

      if (s != t)
        {
          cerr << "Error in setbit, clearbit:" << endl << s << endl << "Pos = " << pos << endl;
          exit (1);
        }
    }
  return 0;
}


static int testshift (int no_of_tests)
{
  LINT s = 0, t;
  unsigned long pos, shft;

  cout << "Testfunction testshift() ..." << endl;

  s.setbit (CLINTMAXBIT - 1);

  t = s;
  t >>= ( CLINTMAXBIT - 1 );
  if (t != 1 || t != ( s >> ( CLINTMAXBIT - 1 )))
    {
      cerr << "Error in shift: t != 1 || t != s" << endl << "t = " << t << endl;
      exit (1);
    }

  if (t << ( CLINTMAXBIT - 1 ) != s || ( t <<= ( CLINTMAXBIT - 1 )) != s)
    {
      cerr << "Error in shift: t != s" << endl << "t = " << t << endl;
      exit (1);
    }


  for (int i = 0; i < no_of_tests; i++)
    {
      s = 0;
      pos = ulrand64_l () % CLINTMAXBIT;
      shft = ulrand64_l () % ( CLINTMAXBIT - pos );

      s.setbit (pos);

      t = s << shft;
      s <<= shft;
      if (!s.testbit (pos + shft) || s != t)
        {
          cerr << "Error in shift:" << endl << s << endl << "pos = " << pos << endl;
          cerr << "pos + shft = " << pos + shft << endl;
          exit (1);
        }

      t = s >> shft;
      s >>= shft;

      if (!s.testbit (pos) || s != t)
        {
          cerr << "Error in shift:" << endl << s << endl << "pos = " << pos << endl;
          cerr << "pos + shft = " << pos + shft << endl;
          exit (1);
        }
    }
  return 0;
}


static int testmax (void)
{
  cout << "Testfunction testmax() ..." << endl;

  LINT a = randl (ulrand64_l () % CLINTMAXBIT);
  LINT b = randl (ulrand64_l () % CLINTMAXBIT);

  if (a > b)
      fswap (a, b);

  if (( min (a, b) != a ) || ( max (a, b) != b ))
    {
      cerr << "Error in min or max before line __LINE__" << endl;
      exit (1);
    }

  a = 1;
  b = 2;

  if (( min (a, LINT (2)) != 1 ) || ( max (LINT (1), b) != 2 ))
    {
      cerr << "Error in min or max before line __LINE__" << endl;
      exit (1);
    }
  return 0;
}



static void report_error (LINT& a, LINT& b, LINT& c, LINT& d, int line)
{
  cerr << "Error in line " << line << endl;
  cerr << "a = " << a << endl;
  cerr << "b = " << b << endl;
  cerr << "c = " << c << endl;
  cerr << "d = " << d << endl;
  abort ();
}


static void report_error (LINT& a, LINT& b, LINT& c, int line)
{
  cerr << "Error in line " << line << endl;
  cerr << "a = " << a << endl;
  cerr << "b = " << b << endl;
  abort ();
}


static void report_error (LINT& a, LINT& b, int line)
{
  cerr << "Error in line " << line << endl;
  cerr << "a = " << a << endl;
  cerr << "b = " << b << endl;
  abort ();
}


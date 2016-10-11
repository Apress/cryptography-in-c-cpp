//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module testchin.cpp     Revision: 10.07.2002                               */
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
#define CLINTRNDLN (1 + ulrand64_l() % (MAXTESTLEN/2))

static int simple_tests (void);
static int general_tests (int);

int main ()
{
  cout << "Testmodule " << __FILE__
       << " compiled for FLINT/C-Library Version "
       << verstr_l () << endl;

  initrand64_lt ();
  simple_tests ();
  general_tests (10000);

  cout << "All tests in " << __FILE__ << " passed." << endl;
  return 0;
}


static int simple_tests ()
{
  LINT a, b, c, m, n, o, x, y;
  LINT **koeff = new LINT *[8];
  unsigned int l;

  cout << "Testfunction simple_tests() ..." << endl;

  koeff[0] = &a;
  koeff[1] = &m;
  koeff[2] = &b;
  koeff[3] = &n;

  // Tests with n = 0

  l = CLINTRNDLN;
  a = randl (usrand64_l () % l / 4);
  m = randl (l);
  b = randl (usrand64_l () % l / 4);
  n = 0;

  x = chinrem (2, koeff);
  if (x.Get_Warning_Status () == E_LINT_OK)
      cerr << "Error in friend function chinrem in line " << __LINE__ << endl;

  y = a.chinrem (m, b, n);
  if (y.Get_Warning_Status () == E_LINT_OK)
      cerr << "Error in member function chinrem in line " << __LINE__ << endl;


  // Test with m = 1

  do
    {
      l = CLINTRNDLN;
    }
  while (4 > l);

  a = randl (usrand64_l () % l / 4);
  m = 1;
  b = 0;
  n = randl (l);

  x = chinrem (2, koeff);
  if (x.Get_Warning_Status () == E_LINT_ERR)
      cerr << "Error in friend function chinrem in line " << __LINE__ << endl;

  y = a.chinrem (m, b, n);
  if (y.Get_Warning_Status () == E_LINT_ERR)
      cerr << "Error in member function chinrem in line " << __LINE__ << endl;

  delete [] koeff;
  return 0;
}


static int general_tests (int no_of_tests)
{
  LINT a, b, c, m, n, o, x, y;
  LINT **koeff = new LINT *[8];
  unsigned int l;

  cout << "Testfunction general_tests() ..." << endl;

  koeff[0] = &a;
  koeff[1] = &m;
  koeff[2] = &b;
  koeff[3] = &n;
  koeff[4] = &c;
  koeff[5] = &o;

  for (int i = 1; i < no_of_tests; i++)
    {
      do
        {
          l = CLINTRNDLN;
          m = randl (l);
        }
      while (10 >= m);

      do
        {
          n = randl (usrand64_l () % l / 3);
        } while (gcd (m, n) != 1);

      do
        {
          o = randl (usrand64_l () % l / 3);
        } while (( gcd (m, o) != 1 ) || ( gcd (n, o) != 1 ));

      a = randl (usrand64_l () % l / 4);
      b = randl (usrand64_l () % l / 4);
      c = randl (usrand64_l () % l / 4);

      x = chinrem (3, koeff);
      if (msub (x, a, m) != 0 || msub (x, b, n) != 0 || msub (x, c, o) != 0)
        {
          cerr << "Error in friend function chinrem" << endl;
          cerr << "x = " << x << endl;
          cerr << "a = " << a;
          cerr << "b = " << b;
          cerr << "c = " << c;
          cerr << "x - a = " << x - a;
          cerr << "x - b = " << x - b;
          cerr << "x - c = " << x - c;
          cerr << "x - a % m = " << ( x - a ) % m;
          cerr << "x - b % n = " << ( x - b ) % n;
          cerr << "x - c % o = " << ( x - c ) % o;
          cerr << "x.status = " << (int) x.Get_Warning_Status () << endl;
          exit (-1);
        }
      else
        {
          // cout << "Solution of congruence:  " <<  x << endl;
        }

      y = a.chinrem (m, b, n);

      if (msub (y, a, m) != 0 || msub (y, b, n) != 0)
        {
          cerr << "Error in member function chinrem" << endl;
          cerr << "y = " << y;
          cerr << "m = " << m;
          cerr << "n = " << n;
          cerr << "y.status = " << (int) y.Get_Warning_Status () << endl;
          cerr << "a = " << a;
          cerr << "b = " << b;
          cerr << "y - a = " << y - a;
          cerr << "y - b = " << y - b;
          cerr << "y - a % m = " << ( y - a ) % m << endl;
          cerr << "y - b % n = " << ( y - b ) % n << endl;
          exit (-1);
        }
      else
        {
          // cout << "Solution of Congruence:  " <<  y << endl;
        }
    }

  delete [] koeff;
  return 0;
}

